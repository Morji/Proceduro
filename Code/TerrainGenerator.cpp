#include "TerrainGenerator.h"


TerrainGenerator::TerrainGenerator(){
	mHeights = 0;
	mMaxHeight = 0;
	mTerrainWidth = mTerrainDepth = 0;

}

TerrainGenerator::~TerrainGenerator(){
	delete [] mHeights;
	mHeights = nullptr;
	
}

void TerrainGenerator::Reset(){
	if (mHeights){
		delete [] mHeights;
		mHeights = nullptr;
	}

	mMaxHeight = 0;
}

bool TerrainGenerator::GenerateFromTGA(char* filename){
	mHeights = 0;

	//store the heightmap to a file
	FILE * fTGA = LoadTGA(filename);

	//compute the mHeights from that file
	if (fTGA != NULL){
		if (!ComputeHeights(fTGA)){
			return false;
		}
	}
	return true;
}

bool TerrainGenerator::GenerateFaultLine(int iterations, float displacementFactor, int width, int depth){
	Reset();

	int index;
	
	mTerrainWidth = width;
	mTerrainDepth = depth;

	mHeights = new float[mTerrainWidth*mTerrainDepth];

	if (!mHeights){
		return false;
	}

	//loop through the terrain and set the hieghts how we want. This is where we generate the terrain
	//in this case I will run a sin-wave through the terrain in one axis.
	float d = sqrt((float)mTerrainWidth*mTerrainWidth + (float)mTerrainDepth*mTerrainDepth);

 	for(int i=0; i<mTerrainWidth; i++){
		for(int j=0; j<mTerrainDepth; j++){	
			index = (mTerrainWidth * i) + j;				
			mHeights[index] = 0;
		}
	}

	for (int i = 0; i < iterations; i++){
		float v = rand();
		float a = sin(v);
		float b = cos(v);
		// rand() / RAND_MAX gives a random number between 0 and 1.
		// therefore c will be a random number between -d/2 and d/2
		float c = RandF() * d - d/2;
		for(int i=0; i<mTerrainWidth; i++){
			for(int j=0; j<mTerrainDepth; j++){
				index = (mTerrainWidth * i) + j;				
				if (a*i + b*j - c > 0){
					mHeights[index] += displacementFactor;
				}
				else{
					mHeights[index] -= displacementFactor;
				}
				if (mHeights[index] > mMaxHeight){
					mMaxHeight = mHeights[index];
				}
			}
		}
	}

	SmoothHeights(0.75f);SmoothHeights(0.75f);

	return true;
}

bool TerrainGenerator::GenerateGridDiamondSquare(int size, float displacementFactor, float roughnessConstant, bool wrap){
	Reset();
	int index;

	size = (int)(pow(2.0f,size))+1;
	
	if (size % 2 == 0)
		size += 1;

	mTerrainWidth = size;
	mTerrainDepth = size;

	mHeights = new float[mTerrainWidth*mTerrainDepth];
	if (!mHeights){
		return false;
	}

 	for(int i=0; i<mTerrainWidth; i++){
		for(int j=0; j<mTerrainDepth; j++){		
			index = (mTerrainWidth * i) + j;
			mHeights[index] = 0.0f;
		}
	}

	int i,j,step;
	float dispM = displacementFactor;
	float r = roughnessConstant; //roughness

	//set the four corners to the default value
	for (int i=0;i<mTerrainWidth;i+=size-1){
		for (int j=0;j<mTerrainDepth;j+=size-1){
			index = i*mTerrainWidth + j;
			mHeights[index] = dispM;
			if (mHeights[index] > mMaxHeight){
				mMaxHeight = mHeights[index];
			}
		}
	}

	for (step = mTerrainWidth-1; step > 1; step /= 2 ) {

		//diamond step
		for (i = 0;i<mTerrainDepth-2;i+=step){
			for(j=0;j<mTerrainWidth-2;j+=step) {	

				index = (i+step/2)*mTerrainWidth + j + step/2;
				mHeights[index] = 
					(GetHeight(i,j) + 
					GetHeight(i+step,j) + 
					GetHeight(i+step,j+step) + 
					GetHeight(i,j+step)) / 4.0f;
				mHeights[index] += RandF(-dispM,dispM);
				if (mHeights[index] > mMaxHeight){
					mMaxHeight = mHeights[index];
				}
			}
		}
		
		//square step
		for (i = 0;i<mTerrainDepth-2;i+=step){
			for(j=0;j<mTerrainWidth-2;j+=step){
				int k,m;
				int x,z;

				x = i + step/2;
				z = j + step/2;

				k = x + step/2;
				m = z;
				if (k == mTerrainDepth-1){
					if (wrap){
						mHeights[k*mTerrainWidth + m] = 
									(GetHeight(k,m+step/2) + 
									 GetHeight(k,m-step/2) + 
									 GetHeight(k-step/2,m) +
									 GetHeight(0,m)) / 4.0f;
					}
					else{
						mHeights[k*mTerrainWidth + m] = 
									(GetHeight(k,m+step/2) + 
									 GetHeight(k,m-step/2) + 
									 GetHeight(k-step/2,m)) / 3.0f;
					}
				}
				else{
					mHeights[k*mTerrainWidth + m] = 
									(GetHeight(k,m+step/2) + 
									GetHeight(k,m-step/2) + 
									GetHeight(k-step/2,m) + 
									GetHeight(k+step/2,m)) / 4.0f;
				}
				mHeights[k*mTerrainWidth + m] += RandF(-dispM,dispM);
				if (mHeights[k*mTerrainWidth + m] > mMaxHeight){
					mMaxHeight = mHeights[k*mTerrainWidth + m];
				}

				k = x;
				m = z + step/2;

				if (m == mTerrainWidth-1)
					if (wrap){
						mHeights[k*mTerrainWidth + m] = 
										(GetHeight(k,m-step/2) + 
										 GetHeight(k-step/2,m) + 
										 GetHeight(k+step/2,m) +
										 GetHeight(k,0)) / 4.0f;
					}
					else{
						mHeights[k*mTerrainWidth + m] = 
										(GetHeight(k,m-step/2) + 
										 GetHeight(k-step/2,m) + 
										 GetHeight(k+step/2,m)) / 3.0f;
					}
				else{
					mHeights[k*mTerrainWidth + m] = 
									(GetHeight(k,m+step/2) + 
									GetHeight(k,m-step/2) + 
									GetHeight(k-step/2,m) + 
									GetHeight(k+step/2,m)) / 4.0f;
				}
				mHeights[k*mTerrainWidth + m] += RandF(-dispM,dispM);
				if (mHeights[k*mTerrainWidth + m] > mMaxHeight){
					mMaxHeight = mHeights[k*mTerrainWidth + m];
				}
	
				k = x - step/2;
				m = z;
				if (k == 0){
					if (wrap){
						mHeights[k*mTerrainWidth + m] = 
										(GetHeight(k,m+step/2) + 
										 GetHeight(k,m-step/2) + 
										 GetHeight(k+step/2,m) +
										 GetHeight(mTerrainWidth-1,m)) / 4.0f;
					}
					else{
						mHeights[k*mTerrainWidth + m] = 
										(GetHeight(k,m+step/2) + 
										GetHeight(k,m-step/2) + 
										GetHeight(k+step/2,m)) / 3.0f;
					}
					mHeights[k*mTerrainWidth + m] += RandF(-dispM,dispM);
					if (mHeights[k*mTerrainWidth + m] > mMaxHeight){
						mMaxHeight = mHeights[k*mTerrainWidth + m];
					}
				}

				m= z - step/2;
				k = x;
				if (m == 0){
					if (wrap){
						mHeights[k*mTerrainWidth + m] = 
										(GetHeight(k,m+step/2) + 
										GetHeight(k-step/2,m) + 
										GetHeight(k+step/2,m) +
										GetHeight(k+step/2,mTerrainDepth-1)) / 4.0f;
					}
					else{
						mHeights[k*mTerrainWidth + m] = 
										(GetHeight(k,m+step/2) + 
										GetHeight(k-step/2,m) + 
										GetHeight(k+step/2,m)) / 3.0f;
					}
					mHeights[k*mTerrainWidth + m] += RandF(-dispM,dispM);
					if (mHeights[k*mTerrainWidth + m] > mMaxHeight){
						mMaxHeight = mHeights[k*mTerrainWidth + m];
					}
				}				
			}
		}
		dispM *= r;		
	}

	SmoothHeights(0.75f);SmoothHeights(0.75f);

	return true;
}

bool TerrainGenerator::NoiseOverTerrain(float maxHeightToAdd, float smoothnessFactor){
	//make sure there is terrain to noise over
	if (mTerrainWidth < 1 || mTerrainDepth < 1)
		return false;

	mMaxHeight = 0.0f;

	SimplexNoise::Init(rand());

	for(int i = 0; i < mTerrainWidth; ++i){
		for(int j = 0; j < mTerrainDepth; ++j){
			int index = (mTerrainWidth * i) + j;	
			double noise = SimplexNoise::Noise2D(i/smoothnessFactor,j/smoothnessFactor);
			float height = mHeights[index] + (noise*maxHeightToAdd);
			mHeights[index] = height;//place in height data array
			if (height > mMaxHeight){
				mMaxHeight = height;
			}
		}
	}

	return true;
}

FILE * TerrainGenerator::LoadTGA(char* filename){
	FILE * fTGA;										// File pointer to texture file
	fTGA = fopen(filename, "rb");						// Open file for reading

	if(fTGA == NULL){									// If it didn't open....	
		MessageBox(NULL, L"Could not open texture file", L"ERROR", MB_OK);	// Display an error message
		return NULL;														// Exit function
	}

	//we need to see what type of TGA we are loading. 
	if(fread(&Tgaheader, sizeof(TGAHeader), 1, fTGA) == 0){					// Attempt to read 12 byte header from file	
		MessageBox(NULL, L"Could not read file header", L"ERROR", MB_OK);		// If it fails, display an error message 
		if(fTGA != NULL){													// Check to seeiffile is still open		
			fclose(fTGA);													// If it is, close it
		}
		return NULL;														// Exit function
	}

	if(memcmp(UTGAcompare, &Tgaheader, sizeof(Tgaheader)) == 0){			// See if header matches the predefined header
		
		return fTGA;														// If so - return the file
	}
	else{																	// If header matches neither type	
		MessageBox(NULL, L"TGA file be type 2 or type 10 ", L"Invalid Image", MB_OK);	// Display an error
		fclose(fTGA);
		return NULL;																// Exit function
	}

	return NULL;							
}

bool TerrainGenerator::ComputeHeights(FILE * fTGA){
	Reset();
	TGA tga;

	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0){				// Read TGA header											
		MessageBox(NULL, L"Could not read info header", L"ERROR", MB_OK);	// Display error if not read
		if(fTGA != NULL){													// if file is still open		
			fclose(fTGA);													// Close it
		}
		return false;														// Return failure
	}	

	unsigned int terrainTexture_width	= tga.header[1] * 256 + tga.header[0];			// Determine The TGA Width	(highbyte*256+lowbyte)
	unsigned int terrainTexture_height	= tga.header[3] * 256 + tga.header[2];			// Determine The TGA Height	(highbyte*256+lowbyte)
	unsigned int terrainTexture_bpp		= tga.header[4];								// Determine the bits per pixel

	tga.width		= terrainTexture_width;										// Copy width into local structure						
	tga.height		= terrainTexture_height;									// Copy height into local structure
	tga.bpp			= terrainTexture_bpp;										// Copy BPP into local structure	
	
	if((terrainTexture_width <= 0) || (terrainTexture_height <= 0) || ((terrainTexture_bpp != 24) && (terrainTexture_bpp !=32)))	// Make sure all information is valid
	{
		MessageBox(NULL, L"Invalid texture information", L"ERROR", MB_OK);	// Display Error
		if(fTGA != NULL){													// Check if file is still open		
			fclose(fTGA);													// If so, close it
		}
		return false;														// Return failed
	}

	tga.bytesPerPixel	= (tga.bpp / 8);												// Compute the number of BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.width * tga.height);					// Compute the total amout of memory needed to store data
	unsigned char *terrainTexture_imageData	= new unsigned char[tga.imageSize];			// Allocate that much memory

	if(terrainTexture_imageData == NULL){											// If no space was allocated	
		MessageBox(NULL, L"Could not allocate memory for image", L"ERROR", MB_OK);	// Display Error
		fclose(fTGA);														// Close the file
		return false;														// Return failed
	}

	if(fread(terrainTexture_imageData, 1, tga.imageSize, fTGA) != tga.imageSize){	// Attempt to read image data	
		MessageBox(NULL, L"Could not read image data", L"ERROR", MB_OK);			// Display Error
		if(terrainTexture_imageData != NULL){										// If imagedata has data in it		
			free(terrainTexture_imageData);											// Delete data from memory
		}
		fclose(fTGA);														// Close file
		return false;														// Return failed
	}

	mHeights = new float[tga.width*tga.height];
	if (!mHeights){
		return false;
	}

	// Byte Swapping Optimized By Steve Thomas
	for(int cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel){
		terrainTexture_imageData[cswap] ^= terrainTexture_imageData[cswap+2] ^=
		terrainTexture_imageData[cswap] ^= terrainTexture_imageData[cswap+2];
	}

	fclose(fTGA); // Close file*/

	mTerrainWidth = tga.width;
	mTerrainDepth = tga.height;

	///The actual height computing and storing
	for (int i = 0; i < mTerrainWidth; i++){
		for (int j = 0; j < mTerrainDepth; j++){
			float pointHeight = terrainTexture_imageData[(j*mTerrainWidth+i)*3];
			mHeights[j*mTerrainWidth + i] = pointHeight;
			if (mHeights[j*mTerrainWidth + i] > mMaxHeight){
				mMaxHeight = mHeights[j*mTerrainWidth + i];
			}
		}
	}

	delete [] terrainTexture_imageData;
	terrainTexture_imageData = nullptr;

	return true;															
}

//Smooth out each vertex depending on the nearest vertices to it using the float as the smoothing factor on how much to decrease/increase height
void TerrainGenerator::SmoothHeights(float factor){	
	
	mMaxHeight = 0;//reset max height 
	float k = factor;
	for (int i = 1; i < mTerrainWidth; i++){
		for (int j = 0; j < mTerrainDepth; j++){
			int index = j*mTerrainWidth + i;
			mHeights[index] = mHeights[j*mTerrainWidth + i - 1]*(1-k) + mHeights[index] * k;
			if (mHeights[index] > mMaxHeight){
				mMaxHeight = mHeights[index];
			}
		}
	}

	for (int i = mTerrainWidth-2; i < -1; i--){
		for (int j = 0; j < mTerrainDepth; j++){
			int index = j*mTerrainWidth + i;
			mHeights[index] = mHeights[j*mTerrainWidth + i + 1]*(1-k) + mHeights[index] * k;
			if (mHeights[index] > mMaxHeight){
				mMaxHeight = mHeights[index];
			}
		}
	}

	for (int i = 0; i < mTerrainWidth; i++){
		for (int j = 1; j < mTerrainDepth; j++){
			int index = j*mTerrainWidth + i;
			mHeights[index] = mHeights[(j-1)*mTerrainWidth + i]*(1-k) + mHeights[index] * k;
			if (mHeights[index] > mMaxHeight){
				mMaxHeight = mHeights[index];
			}
		}
	}

	for (int i = 0; i < mTerrainWidth; i++){
		for (int j = mTerrainDepth; j < -1; j--){
			int index = j*mTerrainWidth + i;
			mHeights[index] = mHeights[(j+1)*mTerrainWidth + i]*(1-k) + mHeights[index] * k;
			if (mHeights[index] > mMaxHeight){
				mMaxHeight = mHeights[index];
			}
		}
	}
}

float TerrainGenerator::GetHeight(int x, int z){
	if (x > mTerrainWidth-1)
		x -= (mTerrainWidth-1);
	else if (x < 0)
		x += mTerrainWidth-1;
	if (z > mTerrainDepth-1)
		z -= (mTerrainDepth-1);
	else if (z < 0)	
		z += mTerrainDepth-1;
	assert(x>=0 && x < mTerrainWidth);
	assert(z>=0 && z < mTerrainDepth);
	return mHeights[z*mTerrainWidth + x];
}

float TerrainGenerator::GetMaxHeight(){
	return mMaxHeight;
}

int TerrainGenerator::GetWidth(){
	return mTerrainWidth;
}

int TerrainGenerator::GetDepth(){
	return mTerrainDepth;
}