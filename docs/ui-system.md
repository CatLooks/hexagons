# Dimension

Dimension (`Dim`) is an object describing a value that can depend on the size of the element and its parent.

Dimension value consists of:
* pixel value `px` - raw value in pixels,
* parent scalar `ps` - scalar multiplied by the size of parent element,
* element scalar `es` - scalar multiplied by the size of this element

When the dimension is evaluated, the end value is:
```cpp
px + parent_size * ps + element_size * es
```

*Note: `es` part is ignored when evaluating a size dimension.*

## Dimension visualization

The following examples use `parent_size = 20`, `element_size = 4`, and showcase how the dimension of `x` position is evaluated.

* Pixel `px`
```
   0px -> 0                   10px -> 10                 20px -> 20
P: |==================|       |==================|       |==================|
E: ####                                 ####                                 ####
```

* Parent scalar `ps`
```
   0ps -> 20 * 0              0.5ps -> 20 * 0.5          1ps -> 20 * 1
P: |==================|       |==================|       |==================|
E: ####                                 ####                                 ####
```

* Element scalar `ps`
```
   0es -> 4 * 0               1es -> 4 * 1               -0.5es -> 4 * (-0.5)
P: |==================|       |==================|       |==================|
E: ####                           ####                 ####
```

* Alignment scalar `as`

  Alignment scalar is defined as `as = ps - es` and is used to align elements *within* the parent element.

```
   0as -> (20 - 4) * 0       0.5as -> (20 - 4) * 0.5     1as -> (20 - 4) * 1
P: |==================|       |==================|       |==================|
E: ####                               ####                               ####
```
<hr>

# UI system

UI system is a framework that creates an interactive and adaptive interface system.

## System hierarchy

1. `ui::Interface` - contains the entire interface element hierarchy.
2. `ui::Layer` - root elements that are responsible for rendering of UI.
3. `ui::Element` - elements stemming from `ui::Layer`s, render separate UI elements.

## Update order

Every active element will be updated each frame in the following order:
- Recalculation - updates element on-screen position.
    - Pre-recalculation update `onRecalculate()` - executes logic before recalculation.
    - Animation updates - ticks and finishes if needed all animations stored in the element.
    - Bounds recalculation - calculates element on-screen position.
    - Children recalculation - all child elements are recalculated.
- Event handling **(mouse & keyboard)** - processing all incoming events or forwarding events to children.
- Hover events **(mouse position)** - processing all mouse hovering events.

    *Note: event handlers can be attached using `onEvent()`*.

- Updates - executes post-recalculation element updates.

    *Note: update handlers can be attached using `onUpdate()`*.

    *Modification of `bounds` will only take effect during the next frame.*

- Rendering - renders the element:
    - Element drawing - draws only the element.
    - Children drawing - draws all (active) element children.

## Layers

Each interface state consists of multiple `ui::Layer`s.

Each layer sets up interface rendering:
- A layer consists of **texture data** and **text data**.
- Layer rendering can be customized with:
    - Texture - provides a texture for graphical data.
    - Render state - provides rendering settings (for more see `sf::RenderState`).
    - View - ability to optionally override window view.

## Rendering order

Normally, each layer first draws the texture data, then the text data.
```cpp
buffer.text(...); // draw order: 3
buffer.quad(...); // draw order: 1
buffer.quad(...); // draw order: 2
buffer.text(...); // draw order: 4

// | Layer ================= |
// #-------------------------#
// | Graphics ... | Text ... |
```
This means that all text will appear *above* the rest of the UI.

Parts of the layer can be drawn above the text using **buffer forwarding**, which draws all current texture & text data before starting to draw further elements.
```cpp
buffer.text(...); // draw order: 2
buffer.quad(...); // draw order: 1
buffer.forward();
buffer.quad(...); // draw order: 3
buffer.text(...); // draw order: 4

// | Layer =========================================== |
// # --------------------------------------------------#
// | Graphics ... | Text ... | Graphics ... | Text ... |
//                           ^
//                   buffer forwarding
```
Buffer forwarding can be enabled for elements using `ui::Element::forwarding = true`.

<hr>

# Element

A `ui::Element` is an abstract object, from which all other elements are derived.

Each element is described by:
* a bounding box - a rectangle `(x, y, width, height)` where each element is a dimension,
* its child elements.

## Children separation

Element children are split into 2 types: **system items** and **public items**.
- Insertion, replacement, clearing of element groups is separate.
- Modification of **system elements** is only allowed from within the element class.
- Modification of **public elements** can be done externally.
- **System elements** are always processed before **public elements**.

## Overridable behavior

A class derived from `ui::Element` can override:

- Drawing methods:
    * `drawSelf` - draws only the element itself.
    * `drawChildren` - draws all element's children.
- Event handlers:
    * `onTranslate` - executed when UI system changes the language.
    * `onActivate` - executed when the element is activated.
    * `onDeactivate` - executed when the element is deactivated.
    * `onRecalculate` - executed when the element is being recalculated.

## Extra settings

Additionally, every element has configurable flags:
- `ignore` - if `true`, element will ignore all events and not propagate them to children
- `transparent` - if `true`, element will allow elements below to be able to be hovered over.
- `forwarding` - if `true`, the render buffer will be forwarded before the drawing calls.

## Padding & Margin

Every element has `padding` & `margin` fields which consist of integer values in 4 directions.

During recalculation, when the element receives a bounding box in which it is supposed to fit:
- Margin - spacing around the element in the received bounding box,
- Padding - spacing within the element, which is allocated for placing child elements.

```
#----------------------------#   
|    margin                  |   element space = allocated space - margin
|   #--------------------#   |   
|   |    padding         |   |   child space = element space - padding
|   |   #------------#   |   |   
|   |   |            |   |   |   
|   |   |            |   |   |   allocate space is child space of parent element
|   |   |            |   |   |   
|   |   #- children -#   |   |   
|   |                    |   |   element space is provided in draw calls as "self"
|   #- element space ----#   |   
|                            |   element space can also be read with "rect()" method
#- allocated space ----------#   
```

## Event handling

Every element can handle an incoming event with a callback attachment using `onEvent()`.
The callback returns a boolean which indicates *whether the element has "absorbed" the event*.

```cpp
(elements...)->onEvent([=](const Event& evt) {
    if (...) {
        // event is absorbed
        return true;
    };

    // event is not absorbed
    return false;
});
```
Results in:
```
C                 ===============================x=      (absorbtion)
                      ^                          ^
B          =============================================
                      ^                          ^
A =============================================================
                      ^                          ^
              event not absorbed          event absorbed
```

The events are propagated to children (which are displayed above their parents) first:
```
Propagation order: B, A.

B     ===============     (cannot absorb) | B     =======x=======     (can absorb)   
            ^ v                           |              ^            
A ============x========== (can absorb)    | A ======================= (can absorb)
            ^                             |              ^                           
          event                           |            event                         
                                          |
Event absorbed by A after                 | Event absorbed by B.
  B has not absorbed the event.           |   A does not process the event.
```

*Note: an element can process an event and not absorb it.*

Mouse hover events are triggered by the same logic, with `ui::Element::transparent` deciding whether to not absorb the "hover".

## When `bounds` matter

Element's bounding box is not required to be accurately set. For example, element can be drawn outside its bounds just fine.

Element's bounds are used in:
- Child element recalculation,
- Mouse event propagation.

If you don't need these things to work properly, you can omit setting element's size.