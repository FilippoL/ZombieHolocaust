/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    3D Models Demo - Demonstrates loading a model from an .OBJ file.
*/


#include"ModelsDemo.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "objLoader.h"
#include "Mesh.h"
#include "Enemy.h"
#include "Ground.h"
#include "Animation.h"
#include "AudioManager.h"
#include <xnamath.h>
#include <stdio.h>
#include <MMSystem.h>


float moveLeftRight;
float moveBackForward;
float speedleftright, yspeed;

struct TextVertexPos
{
    XMFLOAT3 pos;
    XMFLOAT2 tex0;
};


ModelsDemo::ModelsDemo( ) : textureMapVS_( 0 ), textureMapPS_( 0 ),textTextureMapVS_( 0 ), textTextureMapPS_( 0 ),
							inputLayout_( 0 ), textInputLayout_(0),textVertexBuffer_(0),textColorMapSampler_( 0 ),
                            vertexBuffer_( 0 ),  vertexBuffer2_( 0 ), colorMap_( 0 ), textColorMap_(0), colorMapSampler_( 0 ),_diff(BEGINNER),
                            viewCB_( 0 ), projCB_( 0 ), worldCB_( 0 ), camPosCB_( 0 ), gameState_(PLAY_INTRO), pauseMenuSelection(RETURN),mainmenuselec(PLAY), displayFPS(true)
{
    ZeroMemory( &controller1State_, sizeof( XINPUT_STATE ) );
    ZeroMemory( &prevController1State_, sizeof( XINPUT_STATE ) );
	ZeroMemory(&mouseCurrState, sizeof(DIMOUSESTATE));
	ZeroMemory(&mousePrevState, sizeof(DIMOUSESTATE));

	Audio::Instance()->Init();
	Audio::Instance()->LoadMusic("soundtrack.wav", BACKGROUND_SONG);
	Audio::Instance()->LoadSFX("splat.wav", SPLAT);
	Audio::Instance()->LoadSFX("crash.wav", CRASH);
	Audio::Instance()->LoadSFX("moan.wav", MOAN);


	m_alphaEnableBlendingState = 0;
	m_alphaDisableBlendingState = 0;
	m_depthStencilState = 0;
	m_depthDisabledStencilState = 0;
	frameTime_=0;
	fps_=0;
	
	
}


ModelsDemo::~ModelsDemo( )
{

}


bool ModelsDemo::LoadContent( )
{
	_background_music.SetMusic(BACKGROUND_SONG);
	_background_music.Play();

	_splat.SetSFX(SPLAT);
	_moan.SetSFX(MOAN);
	_crash.SetSFX(CRASH);

    ID3DBlob* vsBuffer = 0;

    bool compileResult = CompileD3DShader( "TextureMap.fx", "VS_Main", "vs_4_0", &vsBuffer );

    if( compileResult == false )
    {
        DXTRACE_MSG( "Error compiling the vertex shader!" );
        return false;
    }

    HRESULT d3dResult;

    d3dResult = d3dDevice_->CreateVertexShader( vsBuffer->GetBufferPointer( ),
        vsBuffer->GetBufferSize( ), 0, &textureMapVS_ );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Error creating the vertex shader!" );

        if( vsBuffer )
            vsBuffer->Release( );

        return false;
    }

    D3D11_INPUT_ELEMENT_DESC solidColorLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    unsigned int totalLayoutElements = ARRAYSIZE( solidColorLayout );

    d3dResult = d3dDevice_->CreateInputLayout( solidColorLayout, totalLayoutElements,
        vsBuffer->GetBufferPointer( ), vsBuffer->GetBufferSize( ), &inputLayout_ );

    //vsBuffer->Release( );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Error creating the input layout!" );
        return false;
    }

    ID3DBlob* psBuffer = 0;

    compileResult = CompileD3DShader( "TextureMap.fx", "PS_Main", "ps_4_0", &psBuffer );

    if( compileResult == false )
    {
        DXTRACE_MSG( "Error compiling pixel shader!" );
        return false;
    }

    d3dResult = d3dDevice_->CreatePixelShader( psBuffer->GetBufferPointer( ),
        psBuffer->GetBufferSize( ), 0, &textureMapPS_ );

    //psBuffer->Release( );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Error creating pixel shader!" );
        return false;
    }

////////////////////////////////////////////text 2d shaders////////////////////////////////////////////////////////////

	//ID3DBlob* vsBuffer = 0;
	vsBuffer = 0;

    compileResult = CompileD3DShader( "TextTextureMap.fx", "VS_Main", "vs_4_0", &vsBuffer );

    if( compileResult == false )
    {
        DXTRACE_MSG( "Error compiling the vertex shader!" );
        return false;
    }

    d3dResult = d3dDevice_->CreateVertexShader( vsBuffer->GetBufferPointer( ),
        vsBuffer->GetBufferSize( ), 0, &textTextureMapVS_ );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Error creating the vertex shader!" );

        if( vsBuffer )
            vsBuffer->Release( );

        return false;
    }

    D3D11_INPUT_ELEMENT_DESC textSolidColorLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    totalLayoutElements = ARRAYSIZE( textSolidColorLayout );

    d3dResult = d3dDevice_->CreateInputLayout( textSolidColorLayout, totalLayoutElements,
        vsBuffer->GetBufferPointer( ), vsBuffer->GetBufferSize( ), &textInputLayout_ );

    vsBuffer->Release( );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Error creating the input layout!" );
        return false;
    }

    //ID3DBlob* psBuffer = 0;
	psBuffer = 0;

    compileResult = CompileD3DShader( "TextTextureMap.fx", "PS_Main", "ps_4_0", &psBuffer );

    if( compileResult == false )
    {
        DXTRACE_MSG( "Error compiling pixel shader!" );
        return false;
    }

    d3dResult = d3dDevice_->CreatePixelShader( psBuffer->GetBufferPointer( ),
        psBuffer->GetBufferSize( ), 0, &textTextureMapPS_ );

    psBuffer->Release( );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Error creating pixel shader!" );
        return false;
    }
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	TextTexture->TextureLoader("fontEX.png", d3dDevice_);


    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Failed to load the texture image!" );
        return false;
    }

	D3D11_SAMPLER_DESC textColorMapDesc;
    ZeroMemory( &textColorMapDesc, sizeof( textColorMapDesc ) );
    textColorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    textColorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    textColorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    textColorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    textColorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    textColorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

    d3dResult = d3dDevice_->CreateSamplerState( &textColorMapDesc, &textColorMapSampler_ );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Failed to create color map sampler state!" );
        return false;
    }

    D3D11_BUFFER_DESC textVertexDesc;
    ZeroMemory( &textVertexDesc, sizeof( textVertexDesc ) );
    textVertexDesc.Usage = D3D11_USAGE_DYNAMIC;
    textVertexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    textVertexDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    const int sizeOfSprite = sizeof( TextVertexPos ) * 6;
    const int maxLetters = 24;

    textVertexDesc.ByteWidth = sizeOfSprite * maxLetters;

    d3dResult = d3dDevice_->CreateBuffer( &textVertexDesc, 0, &textVertexBuffer_ );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Failed to create vertex buffer!" );
        return false;
    }




	//*********************************************************************************/ROAD

	_hg.Initialise();

	for (int i = 0; i < 2; i++)
	{
		
		Road[i].Initialise("road_texture_long.jpg", "Road_long", d3dDevice_); //THIS FUNCTION LOADS TEXTURE OBJ AND LAST HIDDEN PARAMETER IS THE N OF FRAMES, BY DEFAULT IS ONE
		Road[i].SetDistanceFromPlayer(ROAD_SWAP_AFTER);
		Road[i].SetHeight(ROAD_HEIGHT);

		
	}
		/******INITIAL TRANSFORMATIONS******/
		Road[0].Scale(8);
		Road[0].SetPosition(0.0, -4.0, (ROAD_HEIGHT/2) - ROAD_SWAP_AFTER);

		Road[1].Scale(8);
		Road[1].SetPosition(0.0, -4.0, (ROAD_HEIGHT + ROAD_HEIGHT/2) - ROAD_SWAP_AFTER);




	//*********************************************************************************/DESERT
	
	for (int i = 0; i < 2; i++)
	{
		Desert[i].Initialise("Desert_txture.jpg", "Desert", d3dDevice_);//THIS FUNCTION LOADS TEXTURE OBJ AND LAST HIDDEN PARAMETER IS THE N OF FRAMES, BY DEFAULT IS ONE
	
		Desert[i].SetDistanceFromPlayer(DESERT_SWAP_AFTER);
		Desert[i].SetHeight(DESERT_HEIGHT);
	}

	/******INITIAL TRANSFORMATIONS******/
		Desert[0].Scale(8);
		Desert[0].SetPosition(0.0, -4.0, (DESERT_HEIGHT / 2) - DESERT_SWAP_AFTER);

		Desert[1].Scale(8);
		Desert[1].SetPosition(0.0, -4.0, (DESERT_HEIGHT + DESERT_HEIGHT / 2) - DESERT_SWAP_AFTER);




	/*********************************************************************************/
	

	for (int i = 0; i < ZOMBIE_NUMBER; i++)
	{	
		Enemies[i].Initialise("zombie.png", "CompleteAnimZombieOne/zombie", d3dDevice_, 144);//THIS FUNCTION LOADS TEXTURE OBJ AND LAST PARAMETER IS THE N OF FRAMES, BY DEFAULT IS ONE
		Enemies[i].SetRunningIndex(1,99);
		Enemies[i].SetAttackingIndex(101,144);
		Enemies[i].SetDyingIndex(100,100);
		Enemies[i].SetScaleFactor(0.75);

	}


	
	for (int i = 0; i < ZOMBIE_NUMBER; i++)
	{	
		Second_Enemies[i].Initialise("zombie.jpg", "CompleteAnimZombieTwo/zombie2_", d3dDevice_, 50);//THIS FUNCTION LOADS TEXTURE OBJ AND LAST PARAMETER IS THE N OF FRAMES, BY DEFAULT IS ONE
		Second_Enemies[i].SetRunningIndex(1,21);//(24);
		Second_Enemies[i].SetAttackingIndex(34,50);//(35);
		Second_Enemies[i].SetDyingIndex(22,34);//(60);
		Second_Enemies[i].SetScaleFactor(0.55);//0.15
	}




	//*********************************************************************************/LIFEBAR

	_lifes.Restore();


		//THIS FUNCTION LOADS TEXTURE OBJ AND LAST HIDDEN PARAMETER IS THE N OF FRAMES, BY DEFAULT IS ONE
	_lifes.Initialise("lifebar_.png", "HealthBar", d3dDevice_);
	
	

	//*********************************************************************************/WALL

	_wall.Initialise("barrier_texture_BLOOD.jpg", "Barrier", d3dDevice_);


	//*********************************************************************************/CAR

	_car.Initialise("CarTexture_2.jpg", "CarModel2", d3dDevice_);

	

	//*********************************************************************************/SKYDOME
	_enviroment.Initialise("Sky_texture_red.png", "skydome", d3dDevice_);




	//*********************************************************************************/BARRELL

	_barrell.Initialise("barrel_texture_2.jpg", "Barrel90", d3dDevice_);


	

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Failed to load the texture image!" );
        return false;
    }



    D3D11_SAMPLER_DESC colorMapDesc;
    ZeroMemory( &colorMapDesc, sizeof( colorMapDesc ) );
    colorMapDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    colorMapDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    colorMapDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    //colorMapDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    colorMapDesc.MaxLOD = D3D11_FLOAT32_MAX;

    d3dResult = d3dDevice_->CreateSamplerState( &colorMapDesc, &colorMapSampler_ );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Failed to create color map sampler state!" );
        return false;
    }


    D3D11_BUFFER_DESC constDesc;
	ZeroMemory( &constDesc, sizeof( constDesc ) );
	constDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constDesc.ByteWidth = sizeof( XMMATRIX );
	constDesc.Usage = D3D11_USAGE_DEFAULT;

	d3dResult = d3dDevice_->CreateBuffer( &constDesc, 0, &viewCB_ );

	if( FAILED( d3dResult ) )
    {
        return false;
    }

    d3dResult = d3dDevice_->CreateBuffer( &constDesc, 0, &projCB_ );

	if( FAILED( d3dResult ) )
    {
        return false;
    }

    d3dResult = d3dDevice_->CreateBuffer( &constDesc, 0, &worldCB_ );

	if( FAILED( d3dResult ) )
    {
        return false;
    }

	constDesc.ByteWidth = sizeof( XMFLOAT4 );

    d3dResult = d3dDevice_->CreateBuffer( &constDesc, 0, &camPosCB_ );

	if( FAILED( d3dResult ) )
    {
        return false;
    }

	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	//blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	// Create the blend state using the description.
	d3dResult = d3dDevice_->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
	if(FAILED(d3dResult))
	{
		return false;
	}

	// Modify the description to create an alpha disabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	// Create the blend state using the description.
	d3dResult = d3dDevice_->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
	if(FAILED(d3dResult))
	{
		return false;
	}

    projMatrix_ = XMMatrixPerspectiveFovLH( XM_PIDIV2, 800.0f / 600.0f, 0.01f, 1500.0f );
    projMatrix_ = XMMatrixTranspose( projMatrix_ );



    return true;
}


void ModelsDemo::UnloadContent( )
{
    if( colorMapSampler_ ) colorMapSampler_->Release( );
	if (textColorMapSampler_) textColorMapSampler_->Release();
    if( textureMapVS_ ) textureMapVS_->Release( );
    if( textTextureMapPS_ ) textTextureMapPS_->Release( );
    if( textTextureMapVS_ ) textTextureMapVS_->Release( );
    if( textureMapPS_ ) textureMapPS_->Release( );
    if( inputLayout_ ) inputLayout_->Release( );
	if( textInputLayout_ ) textInputLayout_->Release( );
    if( vertexBuffer_ ) vertexBuffer_->Release( );
	if( textVertexBuffer_ ) textVertexBuffer_->Release( );
    if( viewCB_ ) viewCB_->Release( );
    if( projCB_ ) projCB_->Release( );
    if( worldCB_ ) worldCB_->Release( );
	if( camPosCB_ ) camPosCB_->Release( );

	if(m_alphaEnableBlendingState)
	{
		m_alphaEnableBlendingState->Release();
		m_alphaEnableBlendingState = 0;
	}

	if(m_alphaDisableBlendingState)
	{
		m_alphaDisableBlendingState->Release();
		m_alphaDisableBlendingState = 0;
	}


    colorMapSampler_ = 0;
	textColorMapSampler_ = 0;
    textureMapVS_ = 0;
    textureMapPS_ = 0;
	textTextureMapVS_ = 0;
    textTextureMapPS_ = 0;
    inputLayout_ = 0;
	textInputLayout_ = 0;
    vertexBuffer_ = 0;
	textVertexBuffer_ = 0;
    viewCB_ = 0;
    projCB_ = 0;
    worldCB_ = 0;

	
	Audio::Instance()->Unload();
	Audio::Instance()->ShutDown();

	TextTexture->UnloadTexture();

	_wall.Unload();
	_car.Unload();
	_enviroment.Unload();
	_barrell.Unload();

	
	_lifes.Unload();


	for (int i = 0; i < 2; i++)
	{
		Desert[i].Unload();
		Road[i].Unload();
	}
	
	for (int i = 0; i < ZOMBIE_NUMBER; i++)
	{
		Enemies[i].Unload();
		Second_Enemies[i].Unload();
	}

	

}

void ModelsDemo::TurnOnAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn on the alpha blending.
	d3dContext_->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

	return;
}


void ModelsDemo::TurnOffAlphaBlending()
{
	float blendFactor[4];
	

	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	
	// Turn off the alpha blending.
	d3dContext_->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

	return;
}

void ModelsDemo::TurnZBufferOn()
{
	d3dContext_->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}


void ModelsDemo::TurnZBufferOff()
{
	d3dContext_->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}

void ModelsDemo::SetGameState(GameStates state)
{
	gameState_ = state;
}
GameStates ModelsDemo::GetGameState()
{
	return gameState_;
}

void ModelsDemo::Update( float dt )
{

	Audio::Instance()->Update();

	 /********************************************************************************/
	if(gameState_ == START_MENU)
	{
		score = 0;


		if(!(keystate[DIK_ESCAPE] & 0x80)&&(keyPrevState[DIK_ESCAPE] & 0x80))
		{
			//PostQuitMessage(0);
			PostQuitMessage(0);
		}
		if((keystate[DIK_RETURN] & 0x80)&&(mainmenuselec == PLAY))
		{
			//PostQuitMessage(0);
			Reset();
			gameState_ = RUN;
		}
		if((keystate[DIK_RETURN] & 0x80)&&(mainmenuselec == HG))
		{
			//PostQuitMessage(0);
			gameState_ = HIGHSCORE;
		}
		
		if((keystate[DIK_RETURN] & 0x80)&&(mainmenuselec == CRED))
		{
			gameState_ = CREDITS;
		}
		
		if((keystate[DIK_RETURN] & 0x80)&&(mainmenuselec == RESET_HG))
		{
			_hg.Reset();
		}

		if((keystate[DIK_RETURN] & 0x80)&&(mainmenuselec == EXIT))
		{
			PostQuitMessage(0);
		}
		
		if((keystate[DIK_RETURN] & 0x80)&&(mainmenuselec == LOOP_MOVIE))
		{
			gameState_ = PLAY_INTRO;
		}

		if(!(keystate[DIK_RETURN] & 0x80)&&(keyPrevState[DIK_RETURN] & 0x80)&&(mainmenuselec == DIFF))
		{
			_diff++;
		}

		if(
		(!(keystate[DIK_DOWN] & 0x80)&&(keyPrevState[DIK_DOWN] & 0x80))
		||
		(!(keystate[DIK_S] & 0x80)&&(keyPrevState[DIK_S] & 0x80))
		)
		{
			mainmenuselec ++;
		}

		if(
			(!(keystate[DIK_UP] & 0x80)&&(keyPrevState[DIK_UP] & 0x80))
			||
			(!(keystate[DIK_W] & 0x80)&&(keyPrevState[DIK_W] & 0x80))
			)

		{
			mainmenuselec --;
	
		}


	}
	/*****************************************************************************/
	if(gameState_ == PAUSED)
	{
		
		if(!(keystate[DIK_ESCAPE] & 0x80)&&(keyPrevState[DIK_ESCAPE] & 0x80))
		{
			//PostQuitMessage(0);
			gameState_ = RUN;
		}
		if((keystate[DIK_RETURN] & 0x80)&&(pauseMenuSelection == RETURN))
		{
			//PostQuitMessage(0);
			gameState_ = RUN;
		}
		if(!(keystate[DIK_RETURN] & 0x80)&&(keyPrevState[DIK_RETURN] & 0x80)&&(pauseMenuSelection == BACK_TO_MENU))
		{
			//PostQuitMessage(0);
			Reset();
			gameState_ = START_MENU;
		}

	
		
		if((keystate[DIK_RETURN] & 0x80)&&(pauseMenuSelection == QUIT))
		{
			PostQuitMessage(0);
		}
		
	
		
		if(
			(!(keystate[DIK_DOWN] & 0x80)&&(keyPrevState[DIK_DOWN] & 0x80))
			||
			(!(keystate[DIK_S] & 0x80)&&(keyPrevState[DIK_S] & 0x80))
			)
		{
			pauseMenuSelection++;

	
		}
		if(
			(!(keystate[DIK_UP] & 0x80)&&(keyPrevState[DIK_UP] & 0x80))
			||
			(!(keystate[DIK_W] & 0x80)&&(keyPrevState[DIK_W] & 0x80))
			)

		{
			pauseMenuSelection--;

		}
	}
	/*****************************************************************************/
	if (gameState_ == GAMEOVERSTATE)
	{

		if(!(keystate[DIK_ESCAPE] & 0x80)&&(keyPrevState[DIK_ESCAPE] & 0x80))
		{
			
			score = 0;

			gameState_ = START_MENU;		
		}

		
		if((keystate[DIK_RETURN] & 0x80))
		{
			_hg.UpdateHighscore(score);
			gameState_ = HIGHSCORE;
		}
		
	}

	/*****************************************************************************/
	if (gameState_ == CREDITS)
	{

		if(!(keystate[DIK_ESCAPE] & 0x80)&&(keyPrevState[DIK_ESCAPE] & 0x80))
		{
			PostQuitMessage(0);				
		}

		
		if(!(keystate[DIK_RETURN] & 0x80)&&(keyPrevState[DIK_RETURN] & 0x80))
		{
			gameState_ = START_MENU;
		}
		
	}

	/*********************************************************************************/

	if (gameState_ == HIGHSCORE)
	{

		if(!(keystate[DIK_ESCAPE] & 0x80)&&(keyPrevState[DIK_ESCAPE] & 0x80))
		{
			
			gameState_ = START_MENU;
		}

		
		if((keystate[DIK_P] & 0x80))
		{

			score = 0;
			gameState_ = RUN;

		}
		
	}

	/*********************************************************************************/

	if (gameState_ == RUN)
	{
		if (score < 0)
		{
			score = 0;
		}
			_score_counter += 15 * dt;

			float moveSpeed=0.01f;
			float moveSpeed2=5.0f;

			float xRotation=0.0;
			float yRotation=0.0;

			 moveBackForward = 0.0;

			 _game_speed += dt * 1.5;


			int cap_damage;

			

			
			 if (_score_counter > 1.5)
			{
				_score_counter = 0;
			}

			if (_diff == BEGINNER)
			{
				cap_damage = 10;
				score += 0.75 * dt;

			}
			else if (_diff == MEDIUM)
			{
				cap_damage = 9;
				score += 1.0 * dt;

			}
			else if (_diff == MASTER)
			{
				cap_damage = 7.75;
				score += 1.5 * dt;

			}
			
			if (_damage_counter > cap_damage)
			{
				_damage_counter = 0;
			}
			
		
		_damage_counter += 1.5 * dt;


		_car.Update();
		_wall.MoveUpAndDown(-(_game_speed += dt / 75), dt);
		_wall.update(dt);

		if (_car.GetBound().IsColliding(_wall.GetBound()))
		{
			if (_car.GetPosition().z < -55)
			{
				_car.SetPosition(_car.GetPosition().x, _car.GetPosition().y, _wall.GetPosition().z - 50);
			}

			else
			{
				_car.MoveUpAndDown(-500, dt);
			}

		}



		if ((keystate[DIK_LEFT] & 0x80) || (keystate[DIK_A] & 0x80)&& !( _car.Point_collision(_wall.GetBound())))
		{
			
			speedleftright = -100;

			 
			
			_car.MoveLeftRight(speedleftright * dt);

		}
		



		if ((keystate[DIK_RIGHT] & 0x80) || (keystate[DIK_D] & 0x80) && !( _car.Point_collision(_wall.GetBound())))
		{

			speedleftright = 100;


		
			_car.MoveLeftRight(speedleftright * dt);

		}
	


		if ((keystate[DIK_DOWN] & 0x80) || (keystate[DIK_S] & 0x80) && !_car.Point_collision(_wall.GetBound()))
		{

			_car.MoveUpAndDown(-120, dt);
		}

		else 
		{
			if (_car.GetPosition().z < -45 && !_car.GetBound().IsColliding(_wall.GetBound()))
			{
				_car.MoveUpAndDown(240, dt);
			}			
		}

		
		if ((keystate[DIK_UP] & 0x80) || (keystate[DIK_W] & 0x80) && (_car.GetPosition().z > 290))
		{

			_game_speed += dt;
		}


		else
		{
			_game_speed = _game_speed;
		}


		if ((keystate[DIK_UP] & 0x80) || (keystate[DIK_W] & 0x80) && !_car.GetBound().IsColliding(_wall.GetBound()))
		{

			_car.MoveUpAndDown(120, dt);
		}

		
		else 
		{
			if (_car.GetPosition().z > -55 && !_car.GetBound().IsColliding(_wall.GetBound()))
			{
				_car.MoveUpAndDown(-240, dt);
			}
			
		}


		if (_car.GetPosition().z < -camera_.GetDistance())
		{
			if (_car.GetBound().IsColliding(_wall.GetBound()))
			{
				_lifes.SetAlive(false);

			}

			else 
			{
				_car.SetPosition(_car.GetPosition().x, _car.GetPosition().y ,-camera_.GetDistance());
			}
		}

			if (_car.GetPosition().z > 300)
		{
			_car.SetPosition(_car.GetPosition().x, _car.GetPosition().y , 300);
		}


			if (_car.GetPosition().x > 150)
		{
			_car.SetPosition(150, _car.GetPosition().y ,_car.GetPosition().z);
		}

			if (_car.GetPosition().x < -150)
		{
			_car.SetPosition(-150, _car.GetPosition().y ,_car.GetPosition().z);
		}

		if (!(keystate[DIK_ESCAPE] & 0x80) && (keyPrevState[DIK_ESCAPE] & 0x80))
		{
			gameState_ = PAUSED;
		}

		


		if ((mouseCurrState.lX != mousePrevState.lX) || (mouseCurrState.lY != mousePrevState.lY))
		{
			yRotation += mousePrevState.lX * 0.0025f;
			//yRotation += mousePrevState.lX * dt*2.0f;//mouse movement already based on time

			xRotation += mouseCurrState.lY * 0.0025f;
			//xRotation += mouseCurrState.lY * dt*2.0f;/mouse movement already based on time


			if (xRotation > XM_PI / 8)
			{
				xRotation = XM_PI / 8;
			}

			if (xRotation < -(XM_PI / 8))
			{
				xRotation = -(XM_PI / 8);
			}

			mousePrevState = mouseCurrState;
		}




		for (int i = 0; i < 2; i++)
		{
			Road[i].StartMov(-(_game_speed += dt/100), dt);
			Desert[i].StartMov(-(_game_speed += dt/100), dt);
		}

	

		///****************************************************************************************************ENEMY ONE*/

		for (int i = 0; i < ZOMBIE_NUMBER; i++)
		{
			Enemies[i].MoveUpAndDown(-(_game_speed += dt / 85), dt);
			Enemies[i].SetAnimationVelocity(0.15);

			
			/*****************************************************************/
			
			if (!Enemies[i].IsOnCar() && Enemies[i].IsAlive())
			{
				
					
				if (_car.GetBound().IsColliding(Enemies[i].GetBound()))//the area is limiting the zombie to spawn too close to the previous one
				{

					_splat.Play();
					Enemies[i].SetAlive(false);
					score += 1;

				}

				if (Enemies[i].GetPosition().z < (-camera_.GetDistance() - 50))
				{
					Enemies[i].Spawn(Enemies[i-1].GetPosition().z, Enemies[i-1].GetPosition().x, true);
					score -= 0.75;
				}
					

				if (_car.Point_collision(Enemies[i].GetBound()) && _lifes.IsAlive())
				{						
					Enemies[i].IsOnCar() = true;
					score -= 1;	
					_moan.Play();
				}

				
			}

				
		

				
			if (Enemies[i].IsAlive() && Enemies[i].IsOnCar() && _lifes.IsAlive())
			{
				if (Enemies[i].GetBound().IsColliding(_wall.GetBound()))
				{
					_splat.Play();
					Enemies[i].SetAlive(false);
				}

				if (Enemies[i].GetBound().IsColliding(_barrell.GetBound()))
				{
					_splat.Play();

					Enemies[i].SetAlive(false);
					_barrell.Respawn();
				}
				

				if (_damage_counter > cap_damage)
				{
					_crash.Play();
					_lifes.Damaged();
				}
				
			}



			if (!Enemies[i].IsAlive())
			{
				if (Enemies[i].GetPosition().z < (-camera_.GetDistance() - 50))
				{
					Enemies[i].SetAlive(true);
					Enemies[i].Spawn(Enemies[i - 1].GetPosition().z, Enemies[i - 1].GetPosition().x);
				}
			}




			
			Enemies[i].GoToward(_car.GetPosition());
			
			if (!Enemies[i].IsAlive())
			{
				Enemies[i].SetScaleFactor(3.5);
			}
			else
			{
				Enemies[i].SetScaleFactor(0.75);
			}

			Enemies[i].update(dt);
		}


		/****************************************************************************************************ENEMY TWO*/
				for (int i = 0; i < ZOMBIE_NUMBER; i++)
		{
			Second_Enemies[i].MoveUpAndDown(-(_game_speed += dt / 85), dt);
			
			
			Second_Enemies[i].SetAnimationVelocity(0.075);//0.1

			/*****************************************************************/
			
			if (!Second_Enemies[i].IsOnCar() && Second_Enemies[i].IsAlive())
			{
				
					
				if (_car.GetBound().IsColliding(Second_Enemies[i].GetBound()))//the area is limiting the zombie to spawn too close to the previous one
				{

					_splat.Play();
					Second_Enemies[i].SetAlive(false);
					score += 1;

				}

				if (Second_Enemies[i].GetPosition().z < (-camera_.GetDistance() - 50))
				{
					Second_Enemies[i].Spawn(Second_Enemies[i-1].GetPosition().z, Second_Enemies[i-1].GetPosition().x, true);
					score -= 0.75;
				}
					

				if (_car.Point_collision(Second_Enemies[i].GetBound()) && _lifes.IsAlive())
				{						
					Second_Enemies[i].IsOnCar() = true;
					score -= 1;	
					_moan.Play();
				}

				
			}

				
			if (Second_Enemies[i].IsAlive() && Second_Enemies[i].IsOnCar() && _lifes.IsAlive())
			{
				Second_Enemies[i].SetAnimationVelocity(0.25);

				if (Second_Enemies[i].GetBound().IsColliding(_wall.GetBound()))
				{
					_splat.Play();
					Second_Enemies[i].SetAlive(false);
				}

				if (Second_Enemies[i].GetBound().IsColliding(_barrell.GetBound()))
				{
					_splat.Play();

					Second_Enemies[i].SetAlive(false);
					_barrell.Respawn();
				}
				

				if (_damage_counter > cap_damage)
				{
					_crash.Play();
					_lifes.Damaged();
				}
				
			}



			if (!Second_Enemies[i].IsAlive())
			{
				Second_Enemies[i].SetAnimationVelocity(0.15);
				Second_Enemies[i].OneTimeAnimation();


				if (Second_Enemies[i].GetPosition().z < (-camera_.GetDistance() - 50))
				{
					Second_Enemies[i].SetAlive(true);
					Second_Enemies[i].Spawn(Second_Enemies[i - 1].GetPosition().z, Second_Enemies[i - 1].GetPosition().x);
				}
			}




			
			Second_Enemies[i].GoToward(_car.GetPosition());

			Enemies[i].SetScaleFactor(0.5);

			Second_Enemies[i].update(dt);
		}

		/**************************************/
	

		for (int i = 0; i < 2; i++)
		{
			Road[i].NeedSwap() = false;
			Desert[i].NeedSwap() = false;
		}
		



		_enviroment.update(dt);


		if (_barrell.GetPosition().x < -150 || _barrell.GetPosition().x > 150 )
		{
			_barrell.Respawn();
		}


		if (_car.Point_collision(_barrell.GetBound()) &&  score > 1 && _lifes.IsAlive())
		{
			_crash.Play();

			for (size_t i = 0; i < _diff+1; i++)//also damage's quantity goes alogn with difficulty
			{
				_lifes.Damaged();
			}
			_barrell.Respawn();
		}

		_lifes.SetPosition(_car.GetPosition().x,_car.GetPosition().y + 45, _car.GetPosition().z);

		_barrell.MoveUpAndDown(-(_game_speed += dt / 75), dt);
	

		_barrell.Roll((_game_speed += dt / 100) * dt);
		
		if (!_lifes.IsAlive())//GAME OVER
		{
			Reset();

			gameState_ = GAMEOVERSTATE;
		}

		_lifes.update();

		XMFLOAT3 lookat (_car.GetPosition().x, 0, -25);

		camera_.SetTarget(lookat);
		camera_.SetRotation(-XM_PIDIV4 + XM_PI/8, XM_PI, 0, 2 * XM_PI);
		if (camera_.GetDistance() < 200)
		{
			camera_.ApplyZoom(zoom);
			zoom += 0.35*dt;
		}
		
		
	}
    memcpy( &prevController1State_, &controller1State_, sizeof( XINPUT_STATE ) );
}


void ModelsDemo::Reset()
{
	/******TRANSFORMATIONS******/
	Road[0].Scale(8);
	Road[0].SetPosition(0.0, -4.0, (ROAD_HEIGHT/2) - ROAD_SWAP_AFTER);

	Road[1].Scale(8);
	Road[1].SetPosition(0.0, -4.0, (ROAD_HEIGHT + ROAD_HEIGHT/2) - ROAD_SWAP_AFTER);

	/******TRANSFORMATIONS******/
	Desert[0].Scale(8);
	Desert[0].SetPosition(0.0, -4.0, (DESERT_HEIGHT / 2) - DESERT_SWAP_AFTER);

	Desert[1].Scale(8);
	Desert[1].SetPosition(0.0, -4.0, (DESERT_HEIGHT + DESERT_HEIGHT / 2) - DESERT_SWAP_AFTER);

	_game_speed = 100;

	for (int i = 0; i < ZOMBIE_NUMBER; i++)
	{

		Enemies[i].Reset();
		Second_Enemies[i].Reset();
		Second_Enemies[i].SetAnimationVelocity(0.1);//0.1
		Enemies[i].SetAnimationVelocity(0.15);
	}



	if (_diff == BEGINNER)
	{
		_game_speed = 100;

		for (size_t i = 0; i < ZOMBIE_NUMBER; i++)
		{
			Enemies[i].SetDifficulty(1);
			Second_Enemies[i].SetDifficulty(1);
		}



	}
	else if (_diff == MEDIUM)
	{
		_game_speed = 150;
		for (size_t i = 0; i < ZOMBIE_NUMBER; i++)
		{
			Enemies[i].SetDifficulty(2);
			Second_Enemies[i].SetDifficulty(2);
		}

	}

	else if (_diff == MASTER)
	{
		for (size_t i = 0; i < ZOMBIE_NUMBER; i++)
		{
			Enemies[i].SetDifficulty(3);
			Second_Enemies[i].SetDifficulty(3);
		}
	
		_game_speed = 235;

	}

	_lifes.Restore();
	_car.SetPosition(0.0,
			0.0,
			500.0);
		

	camera_.SetDistance( 35.0f, 35.0f, 35.0f );
	zoom = 0.0;
}


bool ModelsDemo::DrawString( char* message, float startX, float startY )
{
	//float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

    //d3dContext_->ClearRenderTargetView( backBufferTarget_, clearColor );


    // Size in bytes for a single sprite.
    const int sizeOfSprite = sizeof( TextVertexPos ) * 6;

    // Demo's dynamic buffer setup for max of 24 letters.
    const int maxLetters = 24;

    int length = strlen( message );

    // Clamp for strings too long.
    if( length > maxLetters )
        length = maxLetters;

    // Char's width on screen.
    float charWidth = 32.0f / 800.0f;

    // Char's height on screen.
    float charHeight = 32.0f / 640.0f;
    
    // Char's texel width.
    //float texelWidth = 32.0f / 864.0f;
	float texelWidth = 32.0f / 3072.0f;

    // verts per-triangle (3) * total triangles (2) = 6.
    const int verticesPerLetter = 6;

    D3D11_MAPPED_SUBRESOURCE mapResource;
    HRESULT d3dResult = d3dContext_->Map( textVertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource );

    if( FAILED( d3dResult ) )
    {
        DXTRACE_MSG( "Failed to map resource!" );
        return false;
    }

    // Point to our vertex buffer's internal data.
    TextVertexPos *spritePtr = ( TextVertexPos* )mapResource.pData;

	const int indexSpace = static_cast<char>( ' ' );
    const int indexA = static_cast<char>( 'A' );
    const int indexZ = static_cast<char>( 'Z' );
	const int indexSquare = static_cast<char>( 127 );

    for( int i = 0; i < length; ++i )
    {
        float thisStartX = (startX + ( charWidth * static_cast<float>( i ) ));
        float thisEndX = thisStartX + charWidth;
        float thisEndY = startY + charHeight;

        spritePtr[0].pos = XMFLOAT3( thisEndX,   thisEndY, 1.0f );
        spritePtr[1].pos = XMFLOAT3( thisEndX,   startY,   1.0f );
        spritePtr[2].pos = XMFLOAT3( thisStartX, startY,   1.0f );
        spritePtr[3].pos = XMFLOAT3( thisStartX, startY,   1.0f );
        spritePtr[4].pos = XMFLOAT3( thisStartX, thisEndY, 1.0f );
        spritePtr[5].pos = XMFLOAT3( thisEndX,   thisEndY, 1.0f );

        int texLookup = 0;
        int letter = static_cast<char>( message[i] );

        if( letter < indexSpace || letter > indexSquare )
		{
            // Grab one index past Z, which is a blank space in the texture.
			texLookup = indexSquare;
        }
        else
        {
            // A = 0, B = 1, Z = 25, etc.
			texLookup = ( letter - indexSpace ); 
        }

        float tuStart = 0.0f + ( texelWidth * static_cast<float>( texLookup ) );
        float tuEnd = tuStart + texelWidth;

        spritePtr[0].tex0 = XMFLOAT2( tuEnd, 0.0f );
        spritePtr[1].tex0 = XMFLOAT2( tuEnd, 1.0f );
        spritePtr[2].tex0 = XMFLOAT2( tuStart, 1.0f );
        spritePtr[3].tex0 = XMFLOAT2( tuStart, 1.0f );
        spritePtr[4].tex0 = XMFLOAT2( tuStart, 0.0f );
        spritePtr[5].tex0 = XMFLOAT2( tuEnd, 0.0f );

        spritePtr += 6;
    }

	

    d3dContext_->Unmap( textVertexBuffer_, 0 );
    d3dContext_->Draw( 6 * length, 0 );

    return true;
}

void ModelsDemo::Render( )
{
	

    if( d3dContext_ == 0 )
        return;

	float clearColor[4] = { 0.7f, 0.8f, 1.0f, 1.0f };
    d3dContext_->ClearRenderTargetView( backBufferTarget_, clearColor );
    d3dContext_->ClearDepthStencilView( depthStencilView_, D3D11_CLEAR_DEPTH, 1.0f, 0 );
			
	unsigned int stride = sizeof( VertexPos );
	unsigned int offset = 0;


	if (gameState_ == START_MENU)
	{

		/////////////////////////////////////////////TEXT//////////////////////////////////////////////

		TurnZBufferOff();
		TurnOnAlphaBlending();
	
		stride = sizeof( TextVertexPos );
		offset = 0;

		d3dContext_->IASetInputLayout( textInputLayout_ );
		d3dContext_->IASetVertexBuffers( 0, 1, &textVertexBuffer_, &stride, &offset );
		d3dContext_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		d3dContext_->VSSetShader( textTextureMapVS_, 0, 0 );
		d3dContext_->PSSetShader( textTextureMapPS_, 0, 0 );
		d3dContext_->PSSetShaderResources( 0, 1, TextTexture->GetTexture() );
		d3dContext_->PSSetSamplers( 0, 1, &textColorMapSampler_ );


		DrawString( "ZOMBIE HOLOCAUST", -0.25f, 0.75f );
		if (mainmenuselec == PLAY)
		{
			DrawString( "->PLAY<-", -0.15f, 0.50f );
		}
		else
		{
			DrawString( "PLAY", -0.15f, 0.50f );
		}

		if (mainmenuselec == HG)
		{

			DrawString( "->HIGHSCORE",-0.15f, 0.25f );
			
		}
		else
		{
			DrawString( "HIGHSCORE",-0.15f, 0.25f );
		}
		

		if (mainmenuselec == CRED)
		{
			DrawString( "->CREDITS<-", -0.15f, 0.0f );
		}
		else
		{
			DrawString( "CREDITS", -0.15f, 0.0f );
		}

		if (mainmenuselec == RESET_HG)
		{
			DrawString( "->RESET HIGHSCORE<-", -0.15f, -0.25f );
		}
		else
		{
			DrawString( "RESET HIGHSCORE", -0.15f, -0.25f );
		}
			
		if (mainmenuselec == DIFF)
		{
			DrawString( "->DIFFICULTY:", -0.15f, -0.5f );
		}
		else
		{
			DrawString( "DIFFICULTY:", -0.15f, -0.5f );
		}


		if (mainmenuselec == LOOP_MOVIE)
		{
			DrawString( "->LOOP MOVIE INTRO<-", -0.15f, -0.75f );
		}
		else
		{
			DrawString( "LOOP MOVIE INTRO", -0.15f, -0.75f );
		}


		if (mainmenuselec == EXIT)
		{
			DrawString( "->EXIT<-", -0.15f, -0.90f );
		}
		else
		{
			DrawString( "EXIT", -0.15f, -0.90f );
		}


		if (_diff == BEGINNER)
		{
			(mainmenuselec == DIFF) ? DrawString("BEGINNNER<-", 0.40f, -0.5f) : DrawString("BEGINNNER", 0.40f, -0.5f);

		}
		else if (_diff == MEDIUM)
		{
			(mainmenuselec == DIFF) ? DrawString("NORMAL<-", 0.40f, -0.5f) : DrawString("NORMAL", 0.40f, -0.5f);


		}
		else if(_diff == MASTER)
		{
			(mainmenuselec == DIFF) ? DrawString("LEGEND<-", 0.40f, -0.5f) : DrawString("LEGEND", 0.40f, -0.5f);

		}

		TurnOffAlphaBlending();
		TurnZBufferOn();

		////////////////////////////////////////////////////////////////////////////////////////////////

	}
	
	if (gameState_ == CREDITS)
	{

		/////////////////////////////////////////////TEXT//////////////////////////////////////////////

		TurnZBufferOff();
		TurnOnAlphaBlending();
	
		stride = sizeof( TextVertexPos );
		offset = 0;

		d3dContext_->IASetInputLayout( textInputLayout_ );
		d3dContext_->IASetVertexBuffers( 0, 1, &textVertexBuffer_, &stride, &offset );
		d3dContext_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		d3dContext_->VSSetShader( textTextureMapVS_, 0, 0 );
		d3dContext_->PSSetShader( textTextureMapPS_, 0, 0 );
		d3dContext_->PSSetShaderResources( 0, 1, TextTexture->GetTexture() );
		d3dContext_->PSSetSamplers( 0, 1, &textColorMapSampler_ );


		DrawString( "ZOMBIE HOLOCAUST", -0.15f, 0.75f );
		
		DrawString("Programmers:", -0.15f, 0.55f);
		DrawString("Filippo M. Libardi", -0.05f, 0.45f);

		DrawString("Designers:", -0.15f, 0.15f);
		DrawString("Aneil Singh Notay", -0.05f, 0.05f);
		DrawString("Jason Juul", -0.05f, -0.05f);

		DrawString("RELEASE [ENTER]", -0.25f, -0.65f);
		DrawString("TO GO BACK", -0.25f, -0.75f);


		TurnOffAlphaBlending();
		TurnZBufferOn();

		////////////////////////////////////////////////////////////////////////////////////////////////

	}



	if (gameState_ == HIGHSCORE)
	{
		/////////////////////////////////////////////TEXT//////////////////////////////////////////////

		TurnZBufferOff();
		TurnOnAlphaBlending();
	
		stride = sizeof( TextVertexPos );
		offset = 0;

		d3dContext_->IASetInputLayout( textInputLayout_ );
		d3dContext_->IASetVertexBuffers( 0, 1, &textVertexBuffer_, &stride, &offset );
		d3dContext_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		d3dContext_->VSSetShader( textTextureMapVS_, 0, 0 );
		d3dContext_->PSSetShader( textTextureMapPS_, 0, 0 );
		d3dContext_->PSSetShaderResources( 0, 1, TextTexture->GetTexture());
		d3dContext_->PSSetSamplers( 0, 1, &textColorMapSampler_ );
		char output[70];


		DrawString( "HIGHSCORES:", -0.3f, 0.8f );

		for (int i = 0; i < _hg.GetHighscore().size(); i++)
		{
			sprintf_s(output, "%d ", i+1);
			DrawString(output, -0.4f, 0.65f -((i + 1) * 0.10));


			sprintf_s(output, "%.2f ", _hg.GetHighscore()[i]);
			DrawString(output, 0.05f, 0.65f -((i + 1) * 0.10));

		}
		
		DrawString("[P]", -0.65f, -0.75f);
		DrawString("PLAY", -0.65f, -0.85f);
		

		DrawString("[ESC]", 0.35f, -0.75f);
		DrawString("MAIN MENU", 0.35f, -0.85f);

		TurnOffAlphaBlending();
		TurnZBufferOn();

		////////////////////////////////////////////////////////////////////////////////////////////////
	}



	if (gameState_ == GAMEOVERSTATE)
	{
		/////////////////////////////////////////////TEXT//////////////////////////////////////////////

		TurnZBufferOff();
		TurnOnAlphaBlending();
	
		stride = sizeof( TextVertexPos );
		offset = 0;

		d3dContext_->IASetInputLayout( textInputLayout_ );
		d3dContext_->IASetVertexBuffers( 0, 1, &textVertexBuffer_, &stride, &offset );
		d3dContext_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		d3dContext_->VSSetShader( textTextureMapVS_, 0, 0 );
		d3dContext_->PSSetShader( textTextureMapPS_, 0, 0 );
		d3dContext_->PSSetShaderResources( 0, 1, TextTexture->GetTexture());
		d3dContext_->PSSetSamplers( 0, 1, &textColorMapSampler_ );
		char output[70];


		(score > _hg.GetHighscore()[0])?	DrawString("!!NEW HIGHSCORE!!:", -0.65f, 0.0f) :	
											DrawString("FINAL SCORE:", -0.5f, 0.0f);


		sprintf_s(output, "%.2f ", score);
		DrawString(output, 0.15f, 0.0f);

		
		DrawString("[ENTER]", -0.65f, -0.75f);
		DrawString("SUBMIT HIGHSCORE", -0.65f, -0.85f);
		

		DrawString("[ESC]", 0.35f, -0.75f);
		DrawString("MAIN MENU", 0.35f, -0.85f);

		TurnOffAlphaBlending();
		TurnZBufferOn();

		////////////////////////////////////////////////////////////////////////////////////////////////
	}

	if((gameState_ == RUN)||(gameState_==PAUSED))
	{

		//****************************************************************SET UP GEOMETRY


		d3dContext_->IASetInputLayout( inputLayout_ );
		d3dContext_->IASetVertexBuffers( 0, 1, &vertexBuffer_, &stride, &offset );
		d3dContext_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		d3dContext_->VSSetShader( textureMapVS_, 0, 0 );
		d3dContext_->PSSetShader( textureMapPS_, 0, 0 );

		//****************************************************************ASSIGN MATRICES
		XMMATRIX worldMat = XMMatrixIdentity( );
		worldMat = XMMatrixTranspose( worldMat );

		XMMATRIX viewMat = camera_.GetViewMatrix( );
		viewMat = XMMatrixTranspose( viewMat );

		//XMFLOAT3 cameraPos = camera_.GetPosition( );

		d3dContext_->UpdateSubresource( worldCB_, 0, 0, &worldMat, 0, 0 );
		d3dContext_->UpdateSubresource( viewCB_, 0, 0, &viewMat, 0, 0 );
		d3dContext_->UpdateSubresource( projCB_, 0, 0, &projMatrix_, 0, 0 );
		//d3dContext_->UpdateSubresource( camPosCB_, 0, 0, &cameraPos, 0, 0 );

		d3dContext_->VSSetConstantBuffers( 0, 1, &worldCB_ );
		d3dContext_->VSSetConstantBuffers( 1, 1, &viewCB_ );
		d3dContext_->VSSetConstantBuffers( 2, 1, &projCB_ );
		d3dContext_->VSSetConstantBuffers( 3, 1, &camPosCB_ );
			
		//****************************************************************LIFE
		
		worldMat = XMMatrixTranspose(*_lifes.GetMatrix());

		d3dContext_->IASetVertexBuffers(0, 1, _lifes.GetMesh()->GetGeometry(), &stride, &offset);
		d3dContext_->PSSetShaderResources( 0, 1, _lifes.GetTexture()->GetTexture() );

		d3dContext_->UpdateSubresource(worldCB_, 0, 0, _lifes.GetMatrix(), 0, 0);
		d3dContext_->Draw(_lifes.GetMesh()->GetVerts(), 0);
		

		
		//****************************************************************CAR

		worldMat = XMMatrixTranspose(*_car.GetMatrix());


		d3dContext_->IASetVertexBuffers(0, 1, _car.GetMesh()->GetGeometry(), &stride, &offset);
		d3dContext_->PSSetShaderResources( 0, 1, _car.GetTexture()->GetTexture() );

		d3dContext_->UpdateSubresource(worldCB_, 0, 0, _car.GetMatrix(), 0, 0);
		d3dContext_->Draw(_car.GetMesh()->GetVerts(), 0);

		//****************************************************************_enviroment

		worldMat = XMMatrixTranspose(*_enviroment.GetMatrix());	


		d3dContext_->IASetVertexBuffers(0, 1, _enviroment.GetMesh()->GetGeometry(), &stride, &offset);
		d3dContext_->PSSetShaderResources( 0, 1, _enviroment.GetTexture()->GetTexture() );

		d3dContext_->UpdateSubresource(worldCB_, 0, 0, _enviroment.GetMatrix(), 0, 0);
		d3dContext_->Draw(_enviroment.GetMesh()->GetVerts(), 0);


		//****************************************************************BARREL

		worldMat = XMMatrixTranspose(*_barrell.GetMatrix());


		d3dContext_->IASetVertexBuffers(0, 1, _barrell.GetMesh()->GetGeometry(), &stride, &offset);
		d3dContext_->PSSetShaderResources( 0, 1, _barrell.GetTexture()->GetTexture() );

		d3dContext_->UpdateSubresource(worldCB_, 0, 0, _barrell.GetMatrix(), 0, 0);
		d3dContext_->Draw(_barrell.GetMesh()->GetVerts(), 0);


		

		//****************************************************************WALL

		worldMat = XMMatrixTranspose(*_wall.GetMatrix());



		d3dContext_->IASetVertexBuffers(0, 1, _wall.GetMesh()->GetGeometry(), &stride, &offset);
		d3dContext_->PSSetShaderResources( 0, 1, _wall.GetTexture()->GetTexture() );

		d3dContext_->UpdateSubresource(worldCB_, 0, 0, _wall.GetMatrix(), 0, 0);
		d3dContext_->Draw(_wall.GetMesh()->GetVerts(), 0);
				
		/************************************************************/// Enemies
		
		for (int i = 0; i < ZOMBIE_NUMBER; i++)
		{
			worldMat = XMMatrixTranspose(*Enemies[i].GetMatrix());


			d3dContext_->IASetVertexBuffers(0, 1, Enemies[i].GetMesh()->GetGeometry(Enemies[i].GetFrame()), &stride, &offset);
			d3dContext_->PSSetShaderResources( 0, 1, Enemies[i].GetTexture()->GetTexture() );
			
			d3dContext_->UpdateSubresource(worldCB_, 0, 0, Enemies[i].GetMatrix(), 0, 0);
			d3dContext_->Draw(Enemies[i].GetMesh()->GetVerts(Enemies[i].GetFrame()), 0);
		}

		/************************************************************/// Enemies
		
		for (int i = 0; i < ZOMBIE_NUMBER; i++)
		{
			worldMat = XMMatrixTranspose(*Second_Enemies[i].GetMatrix());


			d3dContext_->IASetVertexBuffers(0, 1, Second_Enemies[i].GetMesh()->GetGeometry(Second_Enemies[i].GetFrame()), &stride, &offset);
			d3dContext_->PSSetShaderResources( 0, 1, Second_Enemies[i].GetTexture()->GetTexture() );
			
			d3dContext_->UpdateSubresource(worldCB_, 0, 0, Second_Enemies[i].GetMatrix(), 0, 0);
			d3dContext_->Draw(Second_Enemies[i].GetMesh()->GetVerts(Second_Enemies[i].GetFrame()), 0);
		}
					
		//****************************************************************ROAD
		for (int i = 0; i < 2; i++)
		{
			worldMat = XMMatrixTranspose(*Road[i].GetMatrix());


			d3dContext_->IASetVertexBuffers(0, 1, Road[i].GetMesh()->GetGeometry(), &stride, &offset);
			d3dContext_->PSSetShaderResources( 0, 1, Road[i].GetTexture()->GetTexture() );

			d3dContext_->UpdateSubresource(worldCB_, 0, 0, Road[i].GetMatrix(), 0, 0);
			d3dContext_->Draw(Road[i].GetMesh()->GetVerts(), 0);
		}
		
		//***************************************************************DESERT
		for (int i = 0; i < 2; i++)
		{
			worldMat = XMMatrixTranspose(*Desert[i].GetMatrix());
		
			d3dContext_->IASetVertexBuffers(0, 1, Desert[i].GetMesh()->GetGeometry(), &stride, &offset);
			d3dContext_->PSSetShaderResources( 0, 1, Desert[i].GetTexture()->GetTexture() );

			d3dContext_->UpdateSubresource(worldCB_, 0, 0, Desert[i].GetMatrix(), 0, 0);
			d3dContext_->Draw(Desert[i].GetMesh()->GetVerts(), 0);

		}


	}

	if((gameState_ == RUN))
	{
		/////////////////////////////////////////////TEXT//////////////////////////////////////////////

		TurnZBufferOff();
		TurnOnAlphaBlending();
	
		stride = sizeof( TextVertexPos );
		offset = 0;

		d3dContext_->IASetInputLayout( textInputLayout_ );
		d3dContext_->IASetVertexBuffers( 0, 1, &textVertexBuffer_, &stride, &offset );
		d3dContext_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		d3dContext_->VSSetShader( textTextureMapVS_, 0, 0 );
		d3dContext_->PSSetShader( textTextureMapPS_, 0, 0 );
		d3dContext_->PSSetShaderResources( 0, 1, TextTexture->GetTexture());
		d3dContext_->PSSetSamplers( 0, 1, &textColorMapSampler_ );

	
		char output[64];
#ifdef _DEBUG
		sprintf_s(output, "FPS:%d",fps_);

		DrawString( output, -0.9f, 0.83f);

		sprintf_s(output, "Frame Time:%.6f", frameTime_);

		DrawString( output, -0.9f, 0.6f);


		sprintf_s(output, "Terrain Position: %f", Road[0].GetPosition().z);

		DrawString(output, -0.9f, -0.15f);

		sprintf_s(output, "Enemy Position: %f", Enemies[0].GetPosition().x);

		DrawString(output, -0.9f, -0.30f);
		
		
		sprintf_s(output, "Car Position: %f", _car.GetPosition().x);
		
		DrawString(output, -0.9f, -0.45f);
		

		if (Enemies[0].GetFrame() > 100)
		{
		
			DrawString("scfhbvsdrfdf", -0.9f, -0.35f);
		}

		sprintf_s(output, "Frame Index: %d", Enemies[0].GetFrame());
		DrawString(output, -0.9f, -0.45f);
#endif
		


		sprintf_s(output, "SCORE: %.1f", score);

		DrawString(output, 0.25f, 0.85f);




		TurnOffAlphaBlending();
		TurnZBufferOn();

	}


	if (gameState_ == PAUSED)
	{
		/////////////////////////////////////////////TEXT//////////////////////////////////////////////

		TurnZBufferOff();
		TurnOnAlphaBlending();
	
		stride = sizeof( TextVertexPos );
		offset = 0;

		d3dContext_->IASetInputLayout( textInputLayout_ );
		d3dContext_->IASetVertexBuffers( 0, 1, &textVertexBuffer_, &stride, &offset );
		d3dContext_->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

		d3dContext_->VSSetShader( textTextureMapVS_, 0, 0 );
		d3dContext_->PSSetShader( textTextureMapPS_, 0, 0 );
		d3dContext_->PSSetShaderResources( 0, 1, TextTexture->GetTexture() );
		d3dContext_->PSSetSamplers( 0, 1, &textColorMapSampler_ );


		DrawString( "GAME PAUSED", -0.2f, 0.0f );
		if (pauseMenuSelection == RETURN)
		{
			DrawString( "->Return to Game<-", -0.33f, -0.1f );
		}
		else
		{
			DrawString( "Return to Game", -0.25f, -0.1f );
		}

		
		if (pauseMenuSelection == BACK_TO_MENU)
		{
			DrawString( "->Main menu<-", -0.33f, -0.3f );
		}
		else
		{
			DrawString( "Main menu", -0.25f, -0.3f );
		}

		if (pauseMenuSelection == QUIT)
		{
			DrawString( "->Quit the Game<-", -0.33f, -0.4f );
		}
		else
		{
			DrawString( "Quit the Game", -0.25f, -0.4f );
		}


		TurnOffAlphaBlending();
		TurnZBufferOn();

		////////////////////////////////////////////////////////////////////////////////////////////////
	}

    swapChain_->Present( 0, 0 );
}

