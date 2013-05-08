// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "GraphicsClass.h"

long stop, remaining;//for timer class

GraphicsClass::GraphicsClass(){
	mScreenWidth = mScreenHeight = 0;
	mD3D = nullptr;
	mTerrain = nullptr;
	mCamera = nullptr;
	mInput = nullptr;
	mCpu = nullptr;
	mText = nullptr;
	mFrustum	= nullptr;
	mWater = nullptr;
	mSkydome = nullptr;
	mSun = nullptr;

	mGlowEffect = nullptr;
	mFullScreenWindow = nullptr;
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
	D3DXMATRIX baseViewMatrix;
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

	mGlowEffect = new GlowEffect();
	result = mGlowEffect->Initialize(mD3D,hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize the glow effect.", L"Error", MB_OK);
		return false;
	}

	// Initialize the text object.
	mCamera->Render();
	mCamera->GetViewMatrix(&baseViewMatrix);
	result = mText->Initialize(mD3D->GetDevice(), hwnd, screenWidth, screenHeight, baseViewMatrix);
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
	mCamera->SetPosition(&Vector3f(-86.0f, 85.0f, -107.0f));
	return true;
}

void GraphicsClass::InitCameras(){
	// Create the god camera object.
	mCamera = new GameCamera();
	// Set the initial position of the camera.
	mCamera->SetPosition(&Vector3f(0.0f, 0.0f, -10.0f));
}

void GraphicsClass::InitShaders(HWND hwnd){

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

	mObjectCollection.push_back(mTerrain);

	cout << "Generating Water..." << endl;
	//init water
	mWater = new Water(Vector3f(0,50,0));
	cout << "Initializing Water..." << endl;
	result = mWater->Initialize(mD3D->GetDevice(),hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize mWater object.", L"Error", MB_OK);
	}

	mObjectCollection.push_back(mWater);
}

void GraphicsClass::InitMisc(HWND hwnd){
	bool result;
	cout << "Generating and Initializing Trees..." << endl;
	int numTrees = 2;
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
		result = tree->GenerateTreeSpaceExploration(mD3D->GetDevice(),minGrowthSize,startRadius,maxHeight);
		if(!result){
			MessageBox(hwnd, L"Could not initialize mTree object.", L"Error", MB_OK);
		}
		mTreeCollection.push_back(tree);
		mObjectCollection.push_back(tree);
		cout << i+1 << " out of " << numTrees << " completed" << endl;
	}

	//Init frustum
	mFrustum = new Frustum();
	if (!mFrustum){
		MessageBox(hwnd, L"Could not initialize mFrustum object.", L"Error", MB_OK);
	}	

	//Init skydome
	mSkydome = new Skydome();
	result = mSkydome->Initialize(mD3D->GetDevice(),hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize mSkydome object.", L"Error", MB_OK);
	}
	mObjectCollection.push_back(mSkydome);

	//Init sun
	mSun = new Sun();
	result = mSun->Initialize(mD3D->GetDevice(),hwnd);
	if (!result){
		MessageBox(hwnd, L"Could not initialize mSun object.", L"Error", MB_OK);
	}
	mSun->SetOrbitPoint(&Vector3f(50,0,50));
	mSun->SetOrbitDistance(500.0f);
	Transform *sunTransform = (Transform*)mSun->GetComponent(TRANSFORM);
	sunTransform->scale = Vector3f(20,20,20);
	mObjectCollection.push_back(mSun);
}

void GraphicsClass::Shutdown()
{
	// Release the D3D object.
	if (mD3D){
		mD3D->Shutdown();
		delete mD3D;
		mD3D = nullptr;
	}

	if (mGlowEffect){
		delete mGlowEffect;
		mGlowEffect = nullptr;
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

	if (mCamera){
		delete mCamera;
		mCamera = nullptr;
	}

	//clean-up game objects
	while (!mObjectCollection.empty()){
		BaseGameObject *object = mObjectCollection.back();
		if (object){
			delete object;
			object = nullptr;
		}
		mObjectCollection.pop_back();
	}
	mObjectCollection.clear();

	//clean-up shaders
	while (!mShaderCollection.empty()){
		IShader *shader = mShaderCollection.back();
		if (shader){
			delete shader;
			shader = nullptr;
		}
		mShaderCollection.pop_back();
	}
	mShaderCollection.clear();

	// Release the full screen ortho window object.
	if(mFullScreenWindow){
		mFullScreenWindow->Shutdown();
		delete mFullScreenWindow;
		mFullScreenWindow = 0;
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
	mD3D->BeginScene(0.0f, 0.0f, 0.0f, 0.0f);		
		//result = RenderScene();if(!result){return false;}
		result = BlurRender();if(!result){return false;}
		result = RenderGUI();if(!result){return false;}
	mD3D->EndScene();

	if(!result)	{
		return false;
	}	

	remaining = stop - mTimer.getCurrTime();
	if (remaining > 0){
		Sleep(remaining);		
	}
	return true;
}

bool GraphicsClass::BlurRender(){
	mGlowEffect->PrepareForSceneRender();
		if (!RenderScene()){return false;}
	mGlowEffect->SceneRenderFinished();		
	mGlowEffect->RenderToWindow(mFullScreenWindow);

	return true;
}

bool GraphicsClass::RenderScene(){
	mCamera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mCamera->GetViewMatrix(&mViewMatrix);
	
	mD3D->GetWorldMatrix(mWorldMatrix);
	mD3D->GetProjectionMatrix(mProjectionMatrix);
	mD3D->GetOrthoMatrix(mOrthoMatrix);
	
	mCamera->GetPosition(&mCamPos);

	// Render sky first
	RenderSky();

	// Construct the frustum.
	mFrustum->ConstructFrustum(SCREEN_DEPTH, mProjectionMatrix, mViewMatrix);
	int renderCount = 0;
	
	mTerrain->Render(mD3D, mFrustum,mViewMatrix, mCamPos,mLight,0);

	BoundingBox *objectBox;
	
	for (int i = 0; i < mTreeCollection.size(); i++){
		//check if all objects lie within the frustum
		objectBox = (BoundingBox*)mTreeCollection[i]->GetComponent(BOUNDING_BOX);
		if (objectBox){
			if (mFrustum->CheckBoundingBox(objectBox)){
				renderCount++;
				mTreeCollection[i]->Render(mD3D,mViewMatrix,mCamPos,mLight,0);
			}
		}
		else{
			mTreeCollection[i]->Render(mD3D,mViewMatrix,mCamPos,mLight,0);
		}
	}
	
	RenderWater();

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

void GraphicsClass::RenderSky(){
	Transform *skyDomeTransform = mSkydome->GetTransform();
	skyDomeTransform->position = mCamPos;
	mD3D->TurnCullingOff();	
		mD3D->TurnZBufferOff();	
			mSkydome->Render(mD3D,mViewMatrix);	
			mSun->Render(mD3D,mViewMatrix);
		mD3D->TurnZBufferOn();	
	mD3D->TurnCullingOn();
}

void GraphicsClass::RenderWater(){
	BoundingBox *objectBox = (BoundingBox*)mWater->GetComponent(BOUNDING_BOX);
	if (objectBox){
		if (!mFrustum->CheckBoundingBox(objectBox)){
			return;
		}
	}

	//draw the water
	mD3D->TurnOnAlphaBlending();
	mWater->Render(mD3D,mViewMatrix, mCamPos,mLight,0);
	mD3D->TurnOffAlphaBlending();
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
	//update sky
	mSkydome->Update(dt);
	mSun->Update(dt);
	//update light
	//mSun->GetLightDirection(&mLight.dir);
	mSun->GetAmbientLight(&mLight.ambient);
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
	mText->SetCameraPosition(mCamPos.x,mCamPos.y,mCamPos.z);
}

void GraphicsClass::MousePick(int x, int y){
	// Compute picking ray in view space.
	Vector3f rayOrigin,rayDir;

	ComputeRayFromMouse(x,y,&rayOrigin,&rayDir);
	//if water handled the picking, do not check against anything else
	if (mWater->CanHandlePicking(rayOrigin,rayDir)){
		return;
	}
	Transform *treeToMove = nullptr;
	for (int i = 0; i < mTreeCollection.size(); i++){
		//check if all objects lie within the frustum
		BoundingBox *objectBox = (BoundingBox*)mTreeCollection[i]->GetComponent(BOUNDING_BOX);
		if (objectBox){
			if (objectBox->IntersectsRay(rayOrigin,rayDir)){
				treeToMove = mTreeCollection[i]->GetTransform();
				break;
			}
		}
	}
	if (treeToMove){
		treeToMove->position.x += rayDir.x;
		treeToMove->position.z += rayDir.z;
		treeToMove->position.y = mTerrain->GetHeight(treeToMove->position.x,treeToMove->position.z);
	}
}

void GraphicsClass::ComputeRayFromMouse(int x, int y, Vector3f *outRayOrigin, Vector3f *outRayDir){
	mCamera->GetViewMatrix(&mViewMatrix);
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

	*outRayOrigin = nearPoint;
	*outRayDir = direction;
}