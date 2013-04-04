// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsClass.h"

long stop, remaining;//for timer class

GraphicsClass::GraphicsClass(){
	mScreenWidth = mScreenHeight = 0;
	mD3D = 0;
	mTerrain = 0;
	mCamera = 0;
	mInput = 0;
	mCpu = 0;
	mText = 0;
	mOrthoTexShader = 0;
	mFrustum	= 0;
	mWater = 0;

	mHorizontalBlurShader = 0;
	mVerticalBlurShader = 0;
	m_RenderTexture = 0;
	m_DownSampleTexure = 0;
	m_HorizontalBlurTexture = 0;
	m_VerticalBlurTexture = 0;
	m_UpSampleTexure = 0;
	mFullScreenWindow = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, HINSTANCE hinstance)
{
	time_t timeSeed = 1364476902;
	//time(&timeSeed);
	srand(timeSeed);
	cout << "Started generation with seed " << timeSeed << endl;

	bool result;
	D3DXMATRIX basemViewMatrix;
	char videoCard[128];
	int videoMemory;

	mScreenWidth = screenWidth;
	mScreenHeight = screenHeight;

	mHwnd = hwnd;

	int downSampleWidth, downSampleHeight;
	// Set the size to sample down to.
	downSampleWidth = screenWidth / 2;
	downSampleHeight = screenHeight / 2;

	// prepare timer
	mTimer.reset();

	// Create the Direct3D object.
	mD3D = new D3DClass;
	if(!mD3D){
		return false;
	}

	// Initialize the Direct3D object.
	result = mD3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	mInput = new InputClass;
	if(!mInput){
		return false;
	}

	// Initialize the input object.
	result = mInput->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the cpu object.
	mCpu = new CpuClass;
	if (!mCpu){
		return false;
	}
	mCpu->Initialize();

	// Create the text object.
	mText = new TextClass;
	if(!mText)
	{
		return false;
	}
	
	InitLights();
	InitTerrain(hwnd);
	InitShaders(hwnd);
	InitCameras();
	InitMisc(hwnd);

	// Create the render to texture object.
	m_RenderTexture = new RenderTexture;
	if(!m_RenderTexture){
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(mD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the down sample render to texture object.
	m_DownSampleTexure = new RenderTexture;
	if(!m_DownSampleTexure){
		return false;
	}

	// Initialize the down sample render to texture object.
	result = m_DownSampleTexure->Initialize(mD3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the horizontal blur render to texture object.
	m_HorizontalBlurTexture = new RenderTexture;
	if(!m_HorizontalBlurTexture){
		return false;
	}

	// Initialize the horizontal blur render to texture object.
	result = m_HorizontalBlurTexture->Initialize(mD3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur render to texture object.
	m_VerticalBlurTexture = new RenderTexture;
	if(!m_VerticalBlurTexture){
		return false;
	}

	// Initialize the vertical blur render to texture object.
	result = m_VerticalBlurTexture->Initialize(mD3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the up sample render to texture object.
	m_UpSampleTexure = new RenderTexture;
	if(!m_UpSampleTexure){
		return false;
	}

	// Initialize the up sample render to texture object.
	result = m_UpSampleTexure->Initialize(mD3D->GetDevice(), screenWidth, screenHeight, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the full screen ortho window object.
	mFullScreenWindow = new OrthoWindow;
	if(!mFullScreenWindow){
		return false;
	}

	// Initialize the full screen ortho window object.
	result = mFullScreenWindow->Initialize(mD3D->GetDevice(), -1.0f, 1.0f, -1.0f, 1.0f);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the text object.
	mCamera->Render();
	mCamera->GetViewMatrix(basemViewMatrix);
	result = mText->Initialize(mD3D->GetDevice(), hwnd, screenWidth, screenHeight, basemViewMatrix);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Retrieve the video card information.
	mD3D->GetVideoCardInfo(videoCard, videoMemory);

	// Set the video card information in the text object.
	result = mText->SetVideoCardInfo(videoCard, videoMemory);
	if(!result)
	{
		MessageBox(hwnd, L"Could not set video card info in the text object.", L"Error", MB_OK);
		return false;
	}
	mCamera->SetPosition(-86.0f, 85.0f, -107.0f);
	return true;
}

void GraphicsClass::InitCameras(){
	// Create the god camera object.
	mCamera = new GameCamera();
	// Set the initial position of the camera.
	mCamera->SetPosition(0.0f, 0.0f, -10.0f);
}

void GraphicsClass::InitShaders(HWND hwnd){

	mOrthoTexShader = new OrthoTextureShader();
	shaderCollection.push_back(mOrthoTexShader);

	// Create the horizontal blur shader object.
	mHorizontalBlurShader = new HorizontalBlurShader;
	shaderCollection.push_back(mHorizontalBlurShader);

	// Create the vertical blur shader object.
	mVerticalBlurShader = new VerticalBlurShader;
	shaderCollection.push_back(mVerticalBlurShader);

	// Init all shaders
	for (int i = 0; i < shaderCollection.size(); i++){
		IShader *shader = shaderCollection[i];
		bool result = shader->Initialize(mD3D->GetDevice(),hwnd);
		if (!result){
			MessageBox(hwnd, L"Error while initializing shader ", L"Error", MB_OK);
		}
	}
}

void GraphicsClass::InitLights(){
	// Parallel mLight.
	mLight.dir = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mLight.ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	mLight.diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLight.specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

void GraphicsClass::InitTerrain(HWND hwnd){
	bool result;

	time_t timerStart;
	time(&timerStart); 

	cout << "Generating Terrain..." << endl;
	//create a terrain generator 
	TerrainGenerator *generator = new TerrainGenerator;
	result = generator->GenerateGridDiamondSquare(8,20.0f,0.65f, true);
	if(!result){
		MessageBox(hwnd, L"Could properly generate mTerrain.", L"Error", MB_OK);
	}

	generator->NoiseOverTerrain(10.0f,50.0f);
	
	cout << "Initializing Terrain..." << endl;
	//create mTerrain
	mTerrain = new Terrain;
	result = mTerrain->Initialize(mD3D->GetDevice(),hwnd);
	if(!result){
		MessageBox(hwnd, L"Could not initialize the mTerrain object.", L"Error", MB_OK);
	}
	//supply mTerrain with mTerrain generator
	result = mTerrain->CreateTerrain(generator);
	if(!result){
		MessageBox(hwnd, L"Could properly generate mTerrain.", L"Error", MB_OK);
	}
	delete generator;
	generator = nullptr;

	time_t timerEnd;
	time(&timerEnd); 
	cout << "Terrain generated in " << difftime(timerEnd,timerStart) << " seconds" << endl;

	cout << "Generating Water..." << endl;
	//init water
	mWater = new Water(Vector3f(0,50,0));
	cout << "Initializing Water..." << endl;
	result = mWater->Initialize(mD3D->GetDevice(),hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize mWater object.", L"Error", MB_OK);
	}
}

void GraphicsClass::InitMisc(HWND hwnd){
	bool result;
	cout << "Generating and Initializing Trees..." << endl;
	int numTrees = 5;
	//Generate some trees
	for (int i = 0; i < numTrees; i++){
		Vector3f mTreePos = mTerrain->GetRandomPoint();
		Tree *tree = new Tree(mTreePos);
		result = tree->Initialize(mD3D->GetDevice(),hwnd);
		if(!result){
			MessageBox(hwnd, L"Could not initialize mTree object.", L"Error", MB_OK);
		}
		float minGrowthSize = RandF(0.7f,0.9f);
		float startRadius = RandF(0.0007f,0.001f);
		float maxHeight = RandF(25.0f,45.0f);
		result = tree->GenerateTreeSpaceExploration(minGrowthSize,startRadius,maxHeight);
		if(!result){
			MessageBox(hwnd, L"Could not initialize mTree object.", L"Error", MB_OK);
		}
		mTreeCollection.push_back(tree);
		cout << i+1 << " out of " << numTrees << " completed" << endl;
	}

	//Init frustum
	mFrustum = new Frustum();
	if (!mFrustum){
		MessageBox(hwnd, L"Could not initialize mFrustum object.", L"Error", MB_OK);
	}	
}

void GraphicsClass::Shutdown()
{
	// Release the D3D object.
	if(mD3D){
		mD3D->Shutdown();
		delete mD3D;
		mD3D = nullptr;
	}

	if (mInput){
		mInput->Shutdown();
		delete mInput;
		mInput = nullptr;
	}

	if (mFrustum){
		delete mFrustum;
		mFrustum = nullptr;
	}

	if (mTerrain){
		delete mTerrain;
		mTerrain = nullptr;
	}

	if (mWater){
		delete mWater;
		mWater = nullptr;
	}

	if (mCamera){
		delete mCamera;
		mCamera = nullptr;
	}

	//clean-up trees
	while (!mTreeCollection.empty()){
		Tree *tree = mTreeCollection.back();
		if (tree){
			delete tree;
			tree = nullptr;
		}
		mTreeCollection.pop_back();
	}

	//clean-up shaders
	while (!shaderCollection.empty()){
		IShader *shader = shaderCollection.back();
		if (shader){
			delete shader;
			shader = nullptr;
		}
		shaderCollection.pop_back();
	}
	shaderCollection.clear();
	// Release the full screen ortho window object.
	if(mFullScreenWindow){
		mFullScreenWindow->Shutdown();
		delete mFullScreenWindow;
		mFullScreenWindow = 0;
	}

	
	// Release the up sample render to texture object.
	if(m_UpSampleTexure){
		m_UpSampleTexure->Shutdown();
		delete m_UpSampleTexure;
		m_UpSampleTexure = 0;
	}

	// Release the vertical blur render to texture object.
	if(m_VerticalBlurTexture){
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}

	// Release the horizontal blur render to texture object.
	if(m_HorizontalBlurTexture){
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}

	// Release the down sample render to texture object.
	if(m_DownSampleTexure){
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}
	
	// Release the render to texture object.
	if(m_RenderTexture){
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	mTimer.tick();

	mCpu->Frame();
	mText->SetFps(mTimer.getFps());
	mText->SetCpu(mCpu->GetCpuPercentage());

	//artificially limit to 60fps - no need for more
	stop = mTimer.getCurrTime() + 15;
	// Otherwise do the frame processing.

	// Read the user input.
	result = mInput->Frame();
	if(!result){
		return false;
	}

	//update scene
	Update(mTimer.getDeltaTime());

	// Render the graphics scene.
	result = Render();
	//result = BlurRender();
	
	if(!result)	{
		return false;
	}	

	remaining = stop - mTimer.getCurrTime();
	if (remaining > 0)
		Sleep(remaining);		

	return true;
}

bool GraphicsClass::BlurRender(){
	
	// First render the scene to a render texture.
	RenderSceneToTexture();
	
	// Next down sample the render texture to a smaller sized texture.
	DownSampleTexture();

	// Perform a horizontal blur on the down sampled render texture.
	RenderHorizontalBlurToTexture();

	// Now perform a vertical blur on the horizontal blur render texture.
	RenderVerticalBlurToTexture();

	// Up sample the final blurred render texture to screen size again.
	UpSampleTexture();

	// Render the blurred up sampled render texture to the screen.
	Render2DTextureScene();

	return true;
}

void GraphicsClass::RenderSceneToTexture()
{
	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);
	
	RenderScene();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();

	return;
}

void GraphicsClass::DownSampleTexture()
{
	// Set the render target to be the render to texture.
	m_DownSampleTexure->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_DownSampleTexure->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());

	// Render the ortho window using the texture shader and the render to texture of the scene as the texture resource.
	mOrthoTexShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(),	m_RenderTexture->GetShaderResourceView());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();

	return;
}

void GraphicsClass::RenderHorizontalBlurToTexture()
{
	float screenSizeX;

	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeX = (float)m_HorizontalBlurTexture->GetTextureWidth();
	
	// Set the render target to be the render to texture.
	m_HorizontalBlurTexture->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_HorizontalBlurTexture->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());

	// Render the small ortho window using the horizontal blur shader and the down sampled render to texture resource.
	mHorizontalBlurShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(),m_DownSampleTexure->GetShaderResourceView(), screenSizeX);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();
	
	return;
}

void GraphicsClass::RenderVerticalBlurToTexture()
{
	float screenSizeY;

	// Store the screen height in a float that will be used in the vertical blur shader.
	screenSizeY = (float)m_VerticalBlurTexture->GetTextureHeight();
	
	// Set the render target to be the render to texture.
	m_VerticalBlurTexture->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_VerticalBlurTexture->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());
	
	// Render the small ortho window using the vertical blur shader and the horizontal blurred render to texture resource.
	mVerticalBlurShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(),m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY);

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();
	
	return;
}

void GraphicsClass::UpSampleTexture(){
	// Set the render target to be the render to texture.
	m_UpSampleTexure->SetRenderTarget(mD3D->GetDevice());

	// Clear the render to texture.
	m_UpSampleTexure->ClearRenderTarget(mD3D->GetDevice(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());

	// Render the full screen ortho window using the texture shader and the small sized final blurred render to texture resource.
	mOrthoTexShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(), m_VerticalBlurTexture->GetShaderResourceView());

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	
	// Reset the render target back to the original back buffer and not the render to texture anymore.
	mD3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	mD3D->ResetViewport();

	return;
}

void GraphicsClass::Render2DTextureScene(){

	// Clear the buffers to begin the scene.
	mD3D->BeginScene(1.0f, 0.0f, 0.0f, 0.0f);

	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	mFullScreenWindow->Render(mD3D->GetDevice());

	// Render the full screen ortho window using the texture shader and the full screen sized blurred render to texture resource.
	mOrthoTexShader->Render(mD3D->GetDevice(), mFullScreenWindow->GetIndexCount(), m_UpSampleTexure->GetShaderResourceView());

	RenderGUI();
	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	
	// Present the rendered scene to the screen.
	mD3D->EndScene();

	return;
}

bool GraphicsClass::Render()
{
	mD3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);

	if (!RenderScene()){
		return false;
	}
	if (!RenderGUI()){
		return false;
	}

	mD3D->EndScene();
	return true;
}

bool GraphicsClass::RenderScene(){
	mCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mCamera->GetViewMatrix(mViewMatrix);
	mD3D->GetWorldMatrix(mWorldMatrix);
	mD3D->GetProjectionMatrix(mProjectionMatrix);
	mD3D->GetOrthoMatrix(mOrthoMatrix);
	
	D3DXVECTOR3 camPos;
	mCamera->GetPosition(camPos);

	// Construct the frustum.
	mFrustum->ConstructFrustum(SCREEN_DEPTH, mProjectionMatrix, mViewMatrix);
	int renderCount = 0;
	
	mTerrain->Render(mFrustum,mWorldMatrix,mViewMatrix,mProjectionMatrix, camPos,mLight,0);

	BoundingBox *objectBox;
	
	for (int i = 0; i < mTreeCollection.size(); i++){
		//check if all objects lie within the frustum
		objectBox = mTreeCollection[i]->GetBoundingBox();
		if (objectBox){
			if (mFrustum->CheckBoundingBox(objectBox)){
				renderCount++;
				mTreeCollection[i]->Render(mWorldMatrix,mViewMatrix,mProjectionMatrix, camPos,mLight,0);
			}
		}
		else{
			mTreeCollection[i]->Render(mWorldMatrix,mViewMatrix,mProjectionMatrix, camPos,mLight,0);
		}
	}
	
	//objectBox = mWater->GetBoundingBox();
	//if (objectBox){
		//if (mFrustum->CheckBoundingBox(objectBox)){
			renderCount++;
			mD3D->TurnOnAlphaBlending();
			mWater->Render(mD3D->GetDevice(),mWorldMatrix,mViewMatrix,mProjectionMatrix, camPos,mLight,0);
			mD3D->TurnOffAlphaBlending();
		//}
	//}
	

	if (!mText->SetRenderCount(mTerrain->GetDrawCount())){
		return false;
	}

	return true;
}

bool GraphicsClass::RenderGUI(){
	// Turn off the Z buffer to begin all 2D rendering.
	mD3D->TurnZBufferOff();
		
	// Turn on the alpha blending before rendering the text.
	mD3D->TurnOnAlphaBlending();

	// Render the text user interface elements.
	mText->Render(mD3D->GetDevice(), mWorldMatrix, mOrthoMatrix);

	// Turn off alpha blending after rendering the text.
	mD3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	mD3D->TurnZBufferOn();
	return true;
}

void GraphicsClass::Update(float dt){
	UpdateCamera(dt);
	if (mInput->IsSpacePressed()){
		mD3D->TurnWireframeOn();
	}
	if (mInput->IsPgUpPressed()){
		mD3D->TurnWireframeOff();
	}
	if (mInput->IsPgDownPressed()){
		mD3D->Screenshot();
	}
	if (mInput->IsRightPressed()){
		mWater->SetSimulationState(!mWater->GetSimulationState());
	}
	if (mInput->IsMouseLeftPressed()){
		POINT pt;
		GetCursorPos( &pt );
		ScreenToClient( mHwnd, &pt );
		MousePick(pt.x,pt.y);
	}
}

void GraphicsClass::UpdateCamera(float dt){
	if (mInput->IsWPressed()){
		mCamera->moveBackForward += mCamera->camMoveFactor*dt;
	}
	if (mInput->IsSPressed()){
		mCamera->moveBackForward -= mCamera->camMoveFactor*dt;
	}
	if (mInput->IsAPressed()){
		mCamera->moveLeftRight -= mCamera->camMoveFactor*dt;
	}
	if (mInput->IsDPressed()){
		mCamera->moveLeftRight += mCamera->camMoveFactor*dt;
	}
	if (mInput->IsMouseRightPressed()){
		int mouseX,mouseY;
		mInput->GetMouseChange(mouseX,mouseY);
		mCamera->MoveYawPitch(mouseX/500.0f,mouseY/500.0f);
	}
	mCamera->ModifyCamMovement(mInput->GetMouseScroll()/20.0f);
	D3DXVECTOR3 camPos;
	mCamera->GetPosition(camPos); 
	mText->SetCameraPosition(camPos.x,camPos.y,camPos.z);
}

void GraphicsClass::MousePick(int x, int y){
	// Compute picking ray in view space.
	mCamera->GetViewMatrix(mViewMatrix);
	mD3D->GetProjectionMatrix(mProjectionMatrix);
	mD3D->GetWorldMatrix(mWorldMatrix);
	float vx = (+2.0f*x/mScreenWidth  - 1.0f)/mProjectionMatrix(0,0);
	float vy = (-2.0f*y/mScreenHeight + 1.0f)/mProjectionMatrix(1,1);

	D3DXVECTOR3 rayOrigin(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 rayDir(vx, vy, 1.0f);

	D3DXMATRIX inverseV;
	D3DXMatrixInverse(&inverseV, 0, &mViewMatrix);

	D3DXMATRIX inverseW;
	D3DXMatrixInverse(&inverseW, 0, &mWorldMatrix);

	D3DXVec3TransformCoord(&rayOrigin, &rayOrigin, &inverseV);
	D3DXVec3TransformNormal(&rayDir, &rayDir, &inverseV);

	// create 2 positions in screenspace using the cursor position. 0 is as
    // close as possible to the camera, 1 is as far away as possible.
    Vector3f nearSource = Vector3f(x, y, 0.0f);
    Vector3f farSource = Vector3f(x, y, 1.0f);

	// use Viewport.Unproject to tell what those two screen space positions
    // would be in world space. we'll need the projection matrix and view
    // matrix, which we have saved as member variables. We also need a world
    // matrix, which can just be identity.
	Vector3f nearPoint;
	D3DXVec3Unproject(&nearPoint,&nearSource,mD3D->GetViewport(),&mProjectionMatrix,&mViewMatrix,&mWorldMatrix);

    Vector3f farPoint;
	D3DXVec3Unproject(&farPoint,&farSource,mD3D->GetViewport(),&mProjectionMatrix,&mViewMatrix,&mWorldMatrix);

    // find the direction vector that goes from the nearPoint to the farPoint
    // and normalize it....
    Vector3f direction = farPoint - nearPoint;
	D3DXVec3Normalize(&direction,&direction);

	mWater->HandlePicking(nearPoint,direction);
}