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
2. `ui::Layer` - root interface elements that are responsible for rendering the UI.
3. `ui::Element` - elements stemming from `ui::Layer`s, render separate UI elements.

## Update order

Every active element will be updated each frame in the following order:
- Recalculation - updates element on-screen position.
    - Pre-recalculation updates - executes logic before recalculation *(attached using `onRecalculate()`)*.
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

## Extra settings

Additionally, every element has configurable flags:
- `ignore` - if `true`, element will ignore all events and not propagate them to children
- `transparent` - if `true`, element will allow elements below to be able to be hovered over.
- `scissor` - if `true`, everything drawn by element outside of its bounds will be cut off.

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
- Render scissoring,
- Mouse event propagation.

If you don't need these things to work properly, you can omit setting element's size.

# UI Interface

`ui::Interface` is an object that controls the entire UI of the program.

Usage example:
```cpp
// interface instantiation
ui::Interface itf;

// main loop
std::queue<sf::Event> event_queue; // events passed through to UI
while (win.isOpen()) {
    // process events
    while (const auto event = win.pollEvent()) {
        // process events unrelated to UI
        // e.g. window closing
        if (event->is<sf::Event::Closed>()) {
            win.close();
            continue;
        };

        // queue event into event queue
        event_queue.push_back(event);
    };

    // update interface
    itf.recalculate(win.getSize());
    itf.eventq(event_queue);
    itf.update(sf::Mouse::getPosition(win));

    // draw interface
    itf.draw(win);
    win.display();
};
```

## Contexts

Interface is split into contexts, which are separate interfaces that switched between.

By default, a default context is already instantiated. Default context should be used as a backup interface.

New contexts can be instantiated with `ui::Interface::newContext()`:
```cpp
auto context_handle = itf.newContext();
{
    // interface is switched to the new context
    // ui generation here
    // ...
}
```

To switch between contexts, use `ui::Interface::setContext()`.
```cpp
namespace ContextHandles { /* ... */ };

// ...

itf.setContext(ContextHandles::handleName);
```

After creating all contexts, interface should be locked.
```cpp
itf.lock();
```
This will prevent context switching during interface updates which can cause bad results.

To switch between contexts during main loop, use `ui::Interface::switchContext()`:
```
itf.switchContext(...);
```
This will switch the context only when the frame has been finished.

## Layers

Each interface context consists of multiple `ui::Layer`s, which are mainly used to separate parts of UI according to their draw order.

### Intermediate layers

Normally, a layer has the same size as the window. However, each layer can be resized and then mapped on the actual window.

* `setArea(DimVector textureSize, DimRect blitArea)` - adds an intermediate texture step; the layer size will be set to `textureSize`,
  after rendering to the texture, the texture will be rendered on the screen mapped to `blitArea`.
  <br> *Note: `textureSize` can use `es` scalars to scale texture size using `blitArea` size.*
* `removeArea()` - removes the intermediate texture step.

# `ui::Element` class

Element is a base object that renders a part of the UI.

* Public fields:

  | Field | Description |
  |-|-|
  | `ignore: bool` | Whether the element ignores events. |
  | `transparent: bool` | Whether the element propagates hover events. |
  | `bounds: DimRect` | Element bounding box (specified in dimensions). |
  | `margin: Borders` | Element margins. |
  | `padding: Borders` | Element padding. |

* Child management:

  | Function | Description |
  |-|-|
  | `add(Element* e)` | Inserts a new child on the top. |
  | `remove(Element* e)` | Removes the child from the element. |
  | `replace(Element* old, Element* repl)` | Replaces the old element by a new one. |
  | `clear()` | Removes all children from the element. |

  - New instantiated elements must be owning pointers:
  ```cpp
  ui::Element* child = new ui::Element;
  // ...
  parent->add(child);
  ```
  - To manage system elements, use the same methods with an `s` suffix.

* Animations:

  | Function | Description |
  |-|-|
  | `push(Anim* anim)` | Pushes a new animation. Animation will be handled by this element. |
  | `animated() -> bool` | Checks if there are any active animations managed by this element. |

* Overridable methods:

    | Function | Description |
    |-|-|
    | `drawSelf` | Draws only the element itself. |
    | `drawChildren` | Draws all element's children. |
    | `onTranslate` | Executed when UI system changes the language. |
    | `onActivate` | Executed when the element is activated. |
    | `onDeactivate` | Executed when the element is deactivated. |

* Function hooks:

  | Function | Description |
  |-|-|
  | `onEvent(bool(const ui::Event& event) handler)` | Attaches a new event handler to the element (for more see [event handling](##event-handling)). |
  | `onRecalculate(void(const sf::Time& delta) handler)` | Attaches a new pre-recalculation update handler to the element. |
  | `onUpdate(void(const sf::Time& delta) handler)` | Attaches a new pre-draw update handler to the element. |

* General methods (used by interface management):

  | Function | Description |
  |-|-|
  | `recalculate(const sf::Time& delta, sf::IntRect parent_bounds)` | Recalculates the element. |
  | `recalculate()` | Recalculates the element after main recalculation phase *(method for users)*. |
  | `event(Event evt) -> bool` | Sends the event to the element. |
  | `handle(Event evt) -> bool` | Forces the event on the element. |
  | `hover(sf::Vector2i mouse) -> bool` | Updates the hover state of the element. |
  | `update(const sf::Time& delta)` | Updates the element. |
  | `activate()` | Activates the element. |
  | `deactivate()` | Deactivates the element. |
  | `translate()` | Translates the element. |

* State properties:

  | Function | Description |
  |-|-|
  | `active() -> bool` | Checks whether the element is active. |
  | `children() -> const std::list<std::unique_ptr<Element>>&` | Returns public element children. |
  | `parent() -> Element*` | Returns a pointer to parent element. |
  | `position() -> DimVector&` | Returns element position vector reference. |
  | `size() -> DimVector&` | Returns element size vector reference. |
  | `rect() -> sf::IntRect` | Returns element on-screen rectangle (available after recalculation). |

# `ui::Layer` class

Layer is an element that renders a *layer* of the UI.

* Configuration methods:

  | Function | Description |
  |-|-|
  | `setShader(const sf::Shader* shader)` | Sets new layer shader. |
  | `setArea(DimVector size, DimRect area)` | Enables [intermediate texture rendering](###intermediate-layers). |
  | `removeArea()` | Disables intermediate texture rendering. |

* Rendering function:

  `render(sf::RenderTarget& target)` - renders buffer data into a render target.

# `ui::Interface` class

Interface is an object that composed all interface contexts together and provides a common I/O interface to the whole UI.

* Context switching:

  | Function | Description |
  |-|-|
  | `newContext() -> Context` | Creates and switches to a new context. |
  | `setContext(Context ctx)` | Switches to an existing context using a handle generated by `newContext()`. |
  | `defaultContext()` | Switches to a default context. |

* Layer generation:

  `layer() -> Layer` - creates a new rendering layer.

* Forwarding methods:

  | Function | Description |
  |-|-|
  | `recalculate(sf::Vector2u window_size)` | Recalculates the entire UI. |
  | `event(const sf::Event& event)` | Sends an event to every layer. |
  | `update(sf::Vector2i mouse)` | Updates the UI. |
  | `draw(sf::RenderTarget& target)` | Draw the UI. |
  | `translate()` | Translates the entire UI. |

* Other methods:

  `statDraw(void(sf::RenderTarget& target, const RenderStats& stats) call)` - attaches a stats drawing function.
  This function will be called after the entire interface has been drawn.
  `clearColor(sf::Color color)` - sets clearing color before rendering (black by default).

# `ui::RenderBuffer` class

* Drawing methods:

  | Function | Description |
  |-|-|
  | `clear()` | Clears the buffer. |
  | `triangle(...)` | Queues a triangle for drawing. |
  | `quad(...)` | Queues a quad / rectangle for drawing. |
  | `text(const sf::Text& text)` | Queues text for drawing. |
  | `forward(const sf::Texture* texture)` | Forwards the buffer for drawing with a texture. <br> This method should be called at least once inside `drawSelf` overload. <br> Text within the forward will be drawn after all vertex data. |
  | `draw(sf::RenderTarget& target)` | Render the buffer into a render target. |