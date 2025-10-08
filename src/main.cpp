#include <SFML/Graphics.hpp>
#include <iostream>

//needed for sfml to work with imgui:
#include "imgui.h"
#include "imgui-SFML.h"
#include "imguiThemes.h"

//personal include example:
#include "addon.h"


int main()
{
	//creating window:
	sf::RenderWindow window(sf::VideoMode(1000, 500), "SFML works!");

	//ImGui initial setup options - could be used for dev tools:
	ImGui::SFML::Init(window);
	imguiThemes::green();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.FontGlobalScale = 2.f;
	ImGuiStyle &style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg].w = 0.5f;
	style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;


	//sfml setup example:
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	sf::Clock clock;
	
	//testing function from addon.h:
	printer();


	//main loop:
	while (window.isOpen())
	{

		//event handling - sfml:
		sf::Event event;
		while (window.pollEvent(event))
		{
			
			ImGui::SFML::ProcessEvent(window, event);


			if (event.type == sf::Event::Closed)
				window.close();
			else if (event.type == sf::Event::Resized)
			{
				// Adjust the viewport when the window is resized
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}
		}

		//calculate the delta time
		sf::Time deltaTime = clock.restart();
		float deltaTimeSeconds = deltaTime.asSeconds();

		//make sure delta time stays within normal bounds, like between one FPS and zero FPS
		deltaTimeSeconds = std::min(deltaTimeSeconds, 1.f);
		deltaTimeSeconds = std::max(deltaTimeSeconds, 0.f);


		//imgui making use of ui, kind of like sfml clear and drawing:
		ImGui::SFML::Update(window, deltaTime);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, {});
		ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, {});
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
		ImGui::PopStyleColor(2);


		ImGui::Begin("Hello, world!");
		ImGui::Button("Look at this pretty button!");
		ImGui::Text("Hello!");
		ImGui::End();

		//game code goes here....


		//sfml clear + draw for window:
		window.clear();
		window.draw(shape);

		//imgui draw over the window:
		ImGui::SFML::Render(window);


		//display all previous changes:
		window.display();
	}

	//turn imgui off before ending program:
	ImGui::SFML::Shutdown();

	return 0;
}