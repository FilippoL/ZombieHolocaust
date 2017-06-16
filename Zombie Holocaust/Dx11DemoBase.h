/*
    Beginning DirectX 11 Game Programming
    By Allen Sherrod and Wendy Jones

    DX11DemoBase Class - Base class for all demos to derive from.
*/


#ifndef _DEMO_BASE_H_
#define _DEMO_BASE_H_

#include<d3d11.h>
#include<d3dx11.h>
#include<DxErr.h>
#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")




class Dx11DemoBase
{
    public:
        Dx11DemoBase();
        virtual ~Dx11DemoBase();

        bool Initialize( HINSTANCE hInstance, HWND hwnd );
        void Shutdown( );

        bool CompileD3DShader( char* filePath, char* entry,
                               char* shaderModel, ID3DBlob** buffer );

        virtual bool LoadContent( );
        virtual void UnloadContent( );

        virtual void Update( float dt ) = 0;
        virtual void Render( ) = 0;

		void initDInput(HINSTANCE hInstance, HWND hWnd);    // sets up and initializes DirectInput
		void detect_input(void);    // gets the current input state
		void cleanDInput(void);    // closes DirectInput and releases memory



    protected:
        HINSTANCE hInstance_;
        HWND hwnd_;

        D3D_DRIVER_TYPE driverType_;
        D3D_FEATURE_LEVEL featureLevel_;

        ID3D11Device* d3dDevice_;
        ID3D11DeviceContext* d3dContext_;
        IDXGISwapChain* swapChain_;
        ID3D11RenderTargetView* backBufferTarget_;
        
        ID3D11Texture2D* depthTexture_;
        ID3D11DepthStencilView* depthStencilView_;


		LPDIRECTINPUT8 din;    // the pointer to our DirectInput interface
		LPDIRECTINPUTDEVICE8 dinkeyboard;    // the pointer to the keyboard device
		BYTE keystate[256];    // the storage for the key-information
		BYTE keyPrevState[256];    

		LPDIRECTINPUTDEVICE8 DIMouse;
		DIMOUSESTATE mouseCurrState;
		DIMOUSESTATE mousePrevState;

		ID3D11BlendState* m_alphaEnableBlendingState;
		ID3D11BlendState* m_alphaDisableBlendingState;


		ID3D11DepthStencilState* m_depthStencilState;
		ID3D11DepthStencilState* m_depthDisabledStencilState;

};

#endif