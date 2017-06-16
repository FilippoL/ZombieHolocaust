/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    DX11DemoBase Class - Base class for all demos to derive from.
*/


#include"Dx11DemoBase.h"
#include<D3Dcompiler.h>
#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")




Dx11DemoBase::Dx11DemoBase( ) : driverType_( D3D_DRIVER_TYPE_NULL ), featureLevel_( D3D_FEATURE_LEVEL_11_0 ),
                                d3dDevice_( 0 ), d3dContext_( 0 ), swapChain_( 0 ), backBufferTarget_( 0 ),
                                depthTexture_( 0 ), depthStencilView_( 0 )
{

}


Dx11DemoBase::~Dx11DemoBase( )
{
    Shutdown( );
}


bool Dx11DemoBase::Initialize( HINSTANCE hInstance, HWND hwnd )
{
    hInstance_ = hInstance;
    hwnd_ = hwnd;

    RECT dimensions;
    GetClientRect( hwnd, &dimensions );

    unsigned int width = dimensions.right - dimensions.left;
    unsigned int height = dimensions.bottom - dimensions.top;

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE, D3D_DRIVER_TYPE_WARP, D3D_DRIVER_TYPE_SOFTWARE
    };

    unsigned int totalDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };

	//anti-aliasing (x4)
	UINT SampleC = 4;
	UINT SampleQ = 0;

    unsigned int totalFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory( &swapChainDesc, sizeof( swapChainDesc ) );
    swapChainDesc.BufferCount = 1;
    swapChainDesc.BufferDesc.Width = width;
    swapChainDesc.BufferDesc.Height = height;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.OutputWindow = hwnd;
    swapChainDesc.Windowed = true;
    swapChainDesc.SampleDesc.Count = SampleC;//anti-aliasing
    swapChainDesc.SampleDesc.Quality = SampleQ;//anti-aliasing

    unsigned int creationFlags = 0;

#ifdef _DEBUG
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT result;
    unsigned int driver = 0;

    for( driver = 0; driver < totalDriverTypes; ++driver )
    {
        result = D3D11CreateDeviceAndSwapChain( 0, driverTypes[driver], 0, creationFlags,
                                                featureLevels, totalFeatureLevels,
                                                D3D11_SDK_VERSION, &swapChainDesc, &swapChain_,
                                                &d3dDevice_, &featureLevel_, &d3dContext_ );

        if( SUCCEEDED( result ) )
        {
            driverType_ = driverTypes[driver];
            break;
        }
    }

    if( FAILED( result ) )
    {
        DXTRACE_MSG( "Failed to create the Direct3D device!" );
        return false;
    }
/*
	typedef struct D3D11_RASTERIZER_DESC {
  D3D11_FILL_MODE FillMode;
  D3D11_CULL_MODE CullMode;
  BOOL            FrontCounterClockwise;
  INT             DepthBias;
  FLOAT           DepthBiasClamp;
  FLOAT           SlopeScaledDepthBias;
  BOOL            DepthClipEnable;
  BOOL            ScissorEnable;
  BOOL            MultisampleEnable;
  BOOL            AntialiasedLineEnable;
} D3D11_RASTERIZER_DESC;*/

	D3D11_RASTERIZER_DESC RasterizerDesc;
	ZeroMemory( &RasterizerDesc, sizeof( RasterizerDesc ) );
  RasterizerDesc.FillMode = D3D11_FILL_SOLID;
  //RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
  RasterizerDesc.CullMode = D3D11_CULL_FRONT;
  //RasterizerDesc.CullMode = D3D11_CULL_NONE;
  RasterizerDesc.FrontCounterClockwise = TRUE;
  RasterizerDesc.DepthBias = 0;
  RasterizerDesc.DepthBiasClamp = 0;
  RasterizerDesc.SlopeScaledDepthBias = 0;
  RasterizerDesc.DepthClipEnable = TRUE;
  RasterizerDesc.ScissorEnable = FALSE;
  RasterizerDesc.MultisampleEnable = TRUE;
  //RasterizerDesc.AntialiasedLineEnable = TRUE;
  RasterizerDesc.AntialiasedLineEnable = FALSE;

 /* HRESULT CreateRasterizerState(
  [in]   const D3D11_RASTERIZER_DESC *pRasterizerDesc,
  [out]  ID3D11RasterizerState **ppRasterizerState
);*/
  ID3D11RasterizerState *pRasterizerState;
  result = d3dDevice_->CreateRasterizerState(&RasterizerDesc, &pRasterizerState);

  /*void RSSetState(
  [in]  ID3D11RasterizerState *pRasterizerState
);*/
  d3dContext_->RSSetState(pRasterizerState);

    ID3D11Texture2D* backBufferTexture;

    result = swapChain_->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBufferTexture );

    if( FAILED( result ) )
    {
        DXTRACE_MSG( "Failed to get the swap chain back buffer!" );
        return false;
    }

    result = d3dDevice_->CreateRenderTargetView( backBufferTexture, 0, &backBufferTarget_ );

    if( backBufferTexture )
        backBufferTexture->Release( );

    if( FAILED( result ) )
    {
        DXTRACE_MSG( "Failed to create the render target view!" );
        return false;
    }

	/////////////////////////anti-aliasing///////////////anti aliasing////////////////
/*
	HRESULT CheckMultisampleQualityLevels(
  [in]   DXGI_FORMAT Format,
  [in]   UINT SampleCount,
  [out]  UINT *pNumQualityLevels
);*/
	 DXGI_FORMAT Format=DXGI_FORMAT_D24_UNORM_S8_UINT;
	 UINT SampleCount=2;
	 UINT pNumQualityLevels;

	result = d3dDevice_->CheckMultisampleQualityLevels(
	Format,
	SampleCount,
	&pNumQualityLevels);




    D3D11_TEXTURE2D_DESC depthTexDesc;
    ZeroMemory( &depthTexDesc, sizeof( depthTexDesc ) );
    depthTexDesc.Width = width;
    depthTexDesc.Height = height;
    depthTexDesc.MipLevels = 1;
    depthTexDesc.ArraySize = 1;
    depthTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthTexDesc.SampleDesc.Count = SampleC;//anti-aliasing
    depthTexDesc.SampleDesc.Quality = SampleQ;//anti-aliasing
    //depthTexDesc.SampleDesc.Quality = pNumQualityLevels-1;
    depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
    depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthTexDesc.CPUAccessFlags = 0;
    depthTexDesc.MiscFlags = 0;

    result = d3dDevice_->CreateTexture2D( &depthTexDesc, NULL, &depthTexture_ );

    if( FAILED( result ) )
    {
        DXTRACE_MSG( "Failed to create the depth texture!" );
        return false;
    }
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Initialize the description of the depth buffer.
	/*ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	d3dResult = d3dDevice_->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result))
	{
		return false;
	}*/////////////////////////////////////////////////////////////////////////////////////////////


    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof( descDSV ) );
    descDSV.Format = depthTexDesc.Format;
    //descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
    descDSV.Texture2D.MipSlice = 0;

    result = d3dDevice_->CreateDepthStencilView( depthTexture_, &descDSV, &depthStencilView_ );

    if( FAILED( result ) )
    {
        DXTRACE_MSG( "Failed to create the depth stencil target view!" );
        return false;
    }



	///////////////////////////////////////////////////////////////////////////////////////

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	//D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	// Create the depth stencil state.
	result = d3dDevice_->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
	{
		return false;
	}

	d3dContext_->OMSetDepthStencilState(m_depthStencilState, 1);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Set the depth stencil state.

    d3dContext_->OMSetRenderTargets( 1, &backBufferTarget_, depthStencilView_ );


	// Clear the second depth stencil state before setting the parameters.
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
	
	// Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the state using the device.
	result = d3dDevice_->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
	if(FAILED(result))
	{
		return false;
	}

    D3D11_VIEWPORT viewport;
    viewport.Width = static_cast<float>( width );
    viewport.Height = static_cast<float>( height );
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    d3dContext_->RSSetViewports( 1, &viewport );

	initDInput(hInstance, hwnd);    // initialize DirectInput

	//return LoadContent( );
	return true;
}


bool Dx11DemoBase::CompileD3DShader( char* filePath, char* entry, char* shaderModel, ID3DBlob** buffer )
{
    DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBuffer = 0;
    HRESULT result;

    result = D3DX11CompileFromFile( filePath, 0, 0, entry, shaderModel,
        shaderFlags, 0, 0, buffer, &errorBuffer, 0 );

    if( FAILED( result ) )
    {
        if( errorBuffer != 0 )
        {
            OutputDebugStringA( ( char* )errorBuffer->GetBufferPointer( ) );
            errorBuffer->Release( );
        }

        return false;
    }
    
    if( errorBuffer != 0 )
        errorBuffer->Release( );

    return true;
}


bool Dx11DemoBase::LoadContent( )
{
    // Override with demo specifics, if any...
    return true;
}


void Dx11DemoBase::UnloadContent( )
{
    // Override with demo specifics, if any...
}


void Dx11DemoBase::Shutdown( )
{
	
	//cleanDInput();    // release DirectInput

    UnloadContent( );

    if( depthTexture_ ) depthTexture_->Release( );
    if( depthStencilView_ ) depthStencilView_->Release( );
    if( backBufferTarget_ ) backBufferTarget_->Release( );
    if( swapChain_ ) swapChain_->Release( );
    if( d3dContext_ ) d3dContext_->Release( );
    if( d3dDevice_ ) d3dDevice_->Release( );    

    depthTexture_ = 0;
    depthStencilView_ = 0;
    backBufferTarget_ = 0;
    swapChain_ = 0;
    d3dContext_ = 0;
    d3dDevice_ = 0;

	if(m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	
}

// this is the function that initializes DirectInput
void Dx11DemoBase::initDInput(HINSTANCE hInstance, HWND hWnd)
{
    // create the DirectInput interface
    DirectInput8Create(hInstance,    // the handle to the application
                       DIRECTINPUT_VERSION,    // the compatible version
                       IID_IDirectInput8,    // the DirectInput interface version
                       (void**)&din,    // the pointer to the interface
                       NULL);    // COM stuff, so we'll set it to NULL

    // create the keyboard device
    din->CreateDevice(GUID_SysKeyboard,    // the default keyboard ID being used
                      &dinkeyboard,    // the pointer to the device interface
                      NULL);    // COM stuff, so we'll set it to NULL

    // set the data format to keyboard format
    dinkeyboard->SetDataFormat(&c_dfDIKeyboard);

    // set the control we will have over the keyboard
    dinkeyboard->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	din->CreateDevice(GUID_SysMouse,&DIMouse,NULL);

	DIMouse->SetDataFormat(&c_dfDIMouse);
	//DIMouse->SetCooperativeLevel(hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	DIMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
}


// this is the function that gets the latest input data
void Dx11DemoBase::detect_input(void)
{
    // get access if we don't have it already
    dinkeyboard->Acquire();

	memcpy( keyPrevState, keystate, sizeof( keystate ) );
	
    // get the input data
    dinkeyboard->GetDeviceState(256, (LPVOID)keystate);

	DIMouse->Acquire();

	memcpy( &mousePrevState, &mouseCurrState, sizeof( mouseCurrState ) );

	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
}


// this is the function that closes DirectInput
void Dx11DemoBase::cleanDInput(void)
{
    dinkeyboard->Unacquire();    // make sure the keyboard is unacquired
	DIMouse->Unacquire();  
    din->Release();    // close DirectInput before exiting
}
