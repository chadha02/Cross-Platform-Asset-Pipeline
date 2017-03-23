// Header Files
//=============

#include "cMyGame.h"
#include "../../Engine/Platform/Platform.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/UserSettings/UserSettings.h"
#include "../../Engine/Math/Functions.h"
#include <time.h>

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cMyGame::~cMyGame()
{

}

// Inherited Implementation
//=========================

// Initialization / Clean Up
//--------------------------

bool start = false;
bool gameOver = false;
bool youWin = false;
float gameTime = 0.0f;
float speedFactor = 2.0f;
uint8_t hits = 0;
uint8_t hitDigit1 = 0;
uint8_t hitDigit2 = 0;
uint8_t wallHit = 0;

bool eae6320::cMyGame::Initialize()
{

	
	const char * snakeMeshPath = "data/RedSquare.Lua";
	//const char * snakeMeshPath = "data/celing.Lua";
	const char * firstMaterialPath = "data/simpleMaterial.material";
	const char * secondMaterialPath = "data/flashingMaterial.material"; 
	

	const char * gameOverMaterialPath = "data/spriteMaterial.material";
	const char * victoryPath = "data/victory.material";

	//sprite1 = new eae6320::Graphics::cSprite(-0.2f, 0.2f, -0.5f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	sprite1 = new eae6320::Graphics::cSprite(-1.2f, 1.2f, 1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	sprite1->Initialize();

	sprite2 = new eae6320::Graphics::cSprite(-1.0f, -0.8f, 1.0f, 0.8f, 0.0f, 1.0f, 1.0f, 0.0f);
	sprite2->Initialize();
	sprite3 = new eae6320::Graphics::cSprite(-0.8f, -0.6f, 1.0f, 0.8f, 0.0f, 1.0f, 1.0f, 0.0f);
	sprite3->Initialize();
	sprite4 = new eae6320::Graphics::cSprite(0.4f, 0.6f, 1.0f, 0.8f, 0.0f, 1.0f, 1.0f, 0.0f); // heart1
	sprite4->Initialize();
	sprite5 = new eae6320::Graphics::cSprite(0.6f, 0.8f, 1.0f, 0.8f, 0.0f, 1.0f, 1.0f, 0.0f);// heart2
	sprite5->Initialize();
	sprite6 = new eae6320::Graphics::cSprite(0.8f, 1.0f, 1.0f, 0.8f, 0.0f, 1.0f, 1.0f, 0.0f);// heart3
	sprite6->Initialize();
	startSprite = new eae6320::Graphics::cSprite(-1.0f, 1.0f, -0.2f, -1.2f, 0.0f, 1.0f, 1.0f, 0.0f);// start
	startSprite->Initialize();

	snakeMesh.load(snakeMeshPath);
	foodMesh.load(snakeMeshPath);

	material1.Load(firstMaterialPath);
	material2.Load(secondMaterialPath);
	

	gameOverMaterial.Load(gameOverMaterialPath);
	score0Material.Load("data/score0.material");
	score1Material.Load("data/score0.material");
	lifeMaterial.Load("data/life.material");
	startMaterial.Load("data/start.material");
	victoryMaterial.Load(victoryPath);

	eae6320::Math::cVector cam_vect(0.0f, 3.0f, 9.0f);
	eae6320::Math::cQuaternion cam_orient;
	float width = (float)eae6320::UserSettings::JustGetResolutionWidth();
	float height = (float)eae6320::UserSettings::JustGetResolutionHeight();
	float aspectRatio = width / height;
	cam = new Camera(60.0f, 0.1f, 100.0f, aspectRatio, cam_orient, cam_vect);
	Graphics::GetCam(cam);

	foodvect = eae6320::Math::cVector(0.0f, 2.0f, 0.0f); 
	snakevect = eae6320::Math::cVector(0.0f, 5.0f, 0.0f); 

	direction.y = 1.0f;
	srand((unsigned int)time(NULL));
	return true;
}

bool eae6320::cMyGame::CleanUp()
{
	
	snakeMesh.CleanUp();
	foodMesh.CleanUp();
	material1.CleanUp();
	material2.CleanUp();
	

	gameOverMaterial.CleanUp();
	score0Material.CleanUp();
	score1Material.CleanUp();
	lifeMaterial.CleanUp();
	victoryMaterial.CleanUp();
	startMaterial.CleanUp();
	delete sprite1;
	delete sprite2;
	delete sprite3;
	delete sprite4;
	delete sprite5;
	delete sprite6;
	delete cam;
	return true;
}

void eae6320::cMyGame::CallMesh()
{
	if (start)
	{
		gameTime += Time::GetElapsedSecondCount_duringPreviousFrame();
	}
	
	angle += (Time::GetElapsedSecondCount_duringPreviousFrame() * 20);
	Math::cVector normalAxis(0.0f, 1.0f, 0.0f);
	//Math::cQuaternion quat1(Math::ConvertDegreesToRadians(angle), normalAxis);
	Math::cQuaternion quat1(Math::ConvertDegreesToRadians(180), normalAxis);
	Math::cQuaternion quat2;
	Math::cQuaternion quat3(Math::ConvertDegreesToRadians(angle), normalAxis);	
	{
		/*if (UserInput::IsKeyPressed(VK_UP))
		{
			eae6320::Math::cVector upVect(0.0f, -1.0f, 0.0f);
			cam->Move(upVect);
		}
		else if (UserInput::IsKeyPressed(VK_DOWN))
		{
			eae6320::Math::cVector downVect(0.0f, 1.0f, 0.0f);
			cam->Move(downVect);
		}
		else if (UserInput::IsKeyPressed(VK_LEFT))
		{
			eae6320::Math::cVector lefVect(1.0f, 0.0f, 0.0f);
			cam->Move(lefVect);
		}
		else if (UserInput::IsKeyPressed(VK_RIGHT))
		{
			eae6320::Math::cVector rightVect(-1.0f, 0.0f, 0.0f);
			cam->Move(rightVect);
		}*/
		//else if (UserInput::IsKeyPressed(0x57))  ///w
		//{
		//	eae6320::Math::cVector rightVect(0.0f, 0.0f, -1.0f);
		//	cam->Move(rightVect);
		//}
		//else if (UserInput::IsKeyPressed(0x53))  //s
		//{
		//	eae6320::Math::cVector rightVect(0.0f, 0.0f, 1.0f);
		//	cam->Move(rightVect);
		//}

		if (UserInput::IsKeyPressed(VK_SPACE))
		{
			start = true;
		}

		if (start)
		{
			snakevect.x += direction.x * Time::GetElapsedSecondCount_duringPreviousFrame() * speedFactor;
			snakevect.y += direction.y * Time::GetElapsedSecondCount_duringPreviousFrame() * speedFactor;
		}
		if (gameTime > 30.0f)
		{
			speedFactor += 1.0f;
			gameTime = 0.0f;
		}

		 if (UserInput::IsKeyPressed(0x57))
		{
			direction.x = 0.0f;
			direction.y = 1.0f;
		}

		else if (UserInput::IsKeyPressed(0x53))  //s
		{
			direction.x = 0.0f;
			direction.y = -1.0f;
		}

		else if (UserInput::IsKeyPressed(0x41))  //a
		{
			direction.x = -1.0f;
			direction.y = 0.0f;
		}

		else if (UserInput::IsKeyPressed(0x44))  //d
		{
			direction.x = 1.0f;
			direction.y = 0.0f;
		}

	

		//else if (UserInput::IsKeyPressed(0x49))  //I
		//{
		//	vect4.y += 2.0f * Time::GetElapsedSecondCount_duringPreviousFrame() ;
		//}
		//else if (UserInput::IsKeyPressed(0x4B))  //k
		//{
		//	vect4.y -= 2.0f *Time::GetElapsedSecondCount_duringPreviousFrame();
		//}
		//else if (UserInput::IsKeyPressed(0x4A))  //J
		//{
		//	vect4.x -= 2.0f *Time::GetElapsedSecondCount_duringPreviousFrame();
		//}
		//else if (UserInput::IsKeyPressed(0x4C))  //L
		//{
		//	vect4.x += 2.0f *Time::GetElapsedSecondCount_duringPreviousFrame();
		//}
	}
	// bound checks for meshes
	{
		if (snakevect.y < -2.0f)
			snakevect.y = -2.0f;
		if (snakevect.y > 7.9f) // 7.75 limit
			snakevect.y = 7.9f;
		if (snakevect.x < -4.8f)
			snakevect.x = -4.8f;
		if (snakevect.x > 4.8f)
			snakevect.x = 4.8f;



		float distance1 = (snakevect.x - foodvect.x)*(snakevect.x - foodvect.x) + (snakevect.y - foodvect.y)*(snakevect.y - foodvect.y);
		

		if (distance1 < 0.90f)
		{
			foodvect.y = float(rand() % 8 - 2);
			foodvect.x = float(rand() % 6 -5);
			hits++;
		}
		if (snakevect.y > 7.75f || snakevect.y < -1.95f || snakevect.x > 4.75f || snakevect.x < -4.75f)
		{
			snakevect.y = 2.5f;
			snakevect.x = 0.0f;
			wallHit++;
			
		}

		hitDigit1 = hits % 10;
		hitDigit2 = hits / 10;

		if (hitDigit1 == 0)
		{
			score0Material.Load("data/score0.material");
		}
		if (hitDigit1 == 1)
		{
			score0Material.Load("data/score1.material");			
		}
		if (hitDigit1 == 2)
		{
			score0Material.Load("data/score2.material");
		}
		if (hitDigit1 == 3)
		{
			score0Material.Load("data/score3.material");
		}
		if (hitDigit1 == 4)
		{
			score0Material.Load("data/score4.material");
		}
		if (hitDigit1 == 5)
		{
			score0Material.Load("data/score5.material");
		}
		if (hitDigit1 == 6)
		{
			score0Material.Load("data/score6.material");
		}
		if (hitDigit1 == 7)
		{
			score0Material.Load("data/score7.material");
		}
		if (hitDigit1 == 8)
		{
			score0Material.Load("data/score8.material");
		}
		if (hitDigit1 == 9)
		{
			score0Material.Load("data/score9.material");
		}
		if (hitDigit2 == 0)
		{
			score1Material.Load("data/score0.material");
		}
		if (hitDigit2 == 1)
		{
			score1Material.Load("data/score1.material");
		}
		if (hitDigit2 == 2)
		{
			score1Material.Load("data/score2.material");
		}
		if (hitDigit2 == 3)
		{
			score1Material.Load("data/score3.material");
		}
		if (hitDigit2 == 4)
		{
			score1Material.Load("data/score4.material");
		}
		if (hitDigit2 == 5)
		{
			score1Material.Load("data/score5.material");
		}
		if (hitDigit2 == 6)
		{
			score1Material.Load("data/score6.material");
		}
		if (hitDigit2 == 7)
		{
			score1Material.Load("data/score7.material");
		}
		if (hitDigit2 == 8)
		{
			score1Material.Load("data/score8.material");
		}
		if (hitDigit2 == 9)
		{
			score1Material.Load("data/score9.material");
		}
	
	}
	if (hits >= 100)
		youWin = true;
	
	Graphics::GetMesh(&snakeMesh, &material2, snakevect, quat1);
	Graphics::GetMesh(&foodMesh, &material1, foodvect, quat1);

	Graphics::GetSprite(sprite2, &score1Material);
	Graphics::GetSprite(sprite3, &score0Material);
	if (wallHit == 0)
	{
		Graphics::GetSprite(sprite4, &lifeMaterial);
		Graphics::GetSprite(sprite5, &lifeMaterial);
		Graphics::GetSprite(sprite6, &lifeMaterial);
	}
	if (wallHit == 1)
	{
		
		Graphics::GetSprite(sprite5, &lifeMaterial);
		Graphics::GetSprite(sprite6, &lifeMaterial);
	}
	if (wallHit == 2)
	{

		Graphics::GetSprite(sprite6, &lifeMaterial);
	}
	if (wallHit == 3)
	{
		gameOver = true;
	}
	if (!start)
	{
		Graphics::GetSprite(startSprite, &startMaterial);
	}
	if (gameOver)
	{
		Graphics::GetSprite(sprite1, &gameOverMaterial);
	}
	if (youWin)
	{
		Graphics::GetSprite(sprite1, &victoryMaterial);
	}
	
}

