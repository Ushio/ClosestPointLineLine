#include "pr.hpp"
#include <iostream>
#include <memory>

int main()
{
	using namespace pr;

	Config config;
	config.ScreenWidth = 1920;
	config.ScreenHeight = 1080;
	config.SwapInterval = 1;
	Initialize( config );

	Camera3D camera;
	camera.origin = { 2, 2, 2 };
	camera.lookat = { 0, 0, 0 };
	camera.zUp = false;

	double e = GetElapsedTime();

	while( pr::NextFrame() == false )
	{
		if( IsImGuiUsingMouse() == false )
		{
			UpdateCameraBlenderLike( &camera );
		}

		ClearBackground( 0.1f, 0.1f, 0.1f, 1 );

		BeginCamera( camera );

		PushGraphicState();

		DrawGrid( GridAxis::XZ, 1.0f, 10, { 128, 128, 128 } );

		float s = 0.2f;
		static glm::vec3 P0 = { -s, 0, 0.5f };
		static glm::vec3 Q0 = { -s, 0.4, -0.5f };
		static glm::vec3 P1 = { s, 0.4, 0.5f };
		static glm::vec3 Q1 = { s, 0, -0.5f };
		glm::vec3 d0 = Q0 - P0;
		glm::vec3 d1 = Q1 - P1;

		ManipulatePosition( camera, &P0, 0.2f );
		ManipulatePosition( camera, &Q0, 0.2f );
		ManipulatePosition( camera, &P1, 0.2f );
		ManipulatePosition( camera, &Q1, 0.2f );

		DrawText( P0, "P0" );
		DrawText( Q0, "Q0" );
		DrawText( P1, "P1" );
		DrawText( Q1, "Q1" );

		DrawArrow( P0, P0 + d0, 0.02f, { 255, 255, 255 } );
		DrawArrow( P1, P1 + d1, 0.02f, { 255, 255, 255 } );

		// Solve u, v
		glm::vec3 n = glm::cross( d0, d1 );
		float length2n = glm::dot( n, n );
		glm::vec3 n2 = glm::cross( P0 - P1, n );
		float u = glm::dot( n2, d1 ) / length2n;
		float v = glm::dot( n2, d0 ) / length2n;

		// visualize
		static bool showClosestLine = true;
		static bool show_n = false;
		static bool show_ng = false;
		static bool show_grid_ng = false;
		static bool show_nb = false;
		glm::vec3 c0 = P0 + u * d0;
		glm::vec3 c1 = P1 + v * d1;

		if( showClosestLine )
		{
			DrawLine( c0, c1, { 255, 0, 255 }, 2 );
		}

		if( show_n )
		{
			DrawArrow( c1, c1 + n, 0.02f, { 255, 0, 0 } );
		}

		glm::vec3 ng = glm::cross( n, d1 );
		if( show_ng )
		{
			DrawArrow( c1, c1 + ng, 0.02f, { 0, 255, 0 } );
		}

		if( show_grid_ng )
		{
			DrawFreeGrid( c1, glm::normalize( n ) * 0.25f, glm::normalize( d1 ) * 0.25f, 3, { 128, 128, 128 }, 1.0f );
		}

		glm::vec3 nb = glm::cross( n, d0 );
		if( show_nb )
		{
			DrawArrow( c0, c0 + nb, 0.02f, { 0, 0, 255 } );
		}

		// https://en.m.wikipedia.org/wiki/Skew_lines#Nearest_Points
		float uSimple = glm::dot( ng, P1 - P0 ) / glm::dot( ng, d0 );
		float vSimple = glm::dot( nb, P0 - P1 ) / glm::dot( nb, d1 );
		// printf("u %f %f\n", u, uSimple);
		// printf("v %f %f\n", v, vSimple);

		PopGraphicState();
		EndCamera();

		BeginImGui();

		ImGui::SetNextWindowSize( { 500, 800 }, ImGuiCond_Once );
		ImGui::Begin( "Panel" );
		ImGui::Text( "fps = %f", GetFrameRate() );
		ImGui::Checkbox( "showClosestLine", &showClosestLine );
		ImGui::Checkbox( "show_n", &show_n );
		ImGui::Checkbox( "show_ng", &show_ng );
		ImGui::Checkbox( "show_nb", &show_nb );
		ImGui::Checkbox( "show_grid_ng", &show_grid_ng );

		ImGui::Text( "u : %.5f", u );
		ImGui::Text( "v : %.5f", v );

		ImGui::End();

		EndImGui();
	}

	pr::CleanUp();
}
