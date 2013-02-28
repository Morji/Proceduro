#include "IShader.h"


/*The OutputShaderErrorMessage writes out error messages that are generating when compiling 
either vertex shaders or pixel shaders.*/
void IShader::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++){
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	ReleaseCOM(errorMessage);

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

//RenderShader will invoke the HLSL shader program through the technique pointer.
void IShader::RenderShader(ID3D10Device* device, int indexCount)
{
	D3D10_TECHNIQUE_DESC techniqueDesc;

	// Set the input layout.
	device->IASetInputLayout(mLayout);

	// Get the description structure of the technique from inside the shader so it can be used for rendering.
	mTechnique->GetDesc(&techniqueDesc);

	// Go through each pass in the technique (should be just one currently) and render the triangles.
	for(unsigned int i = 0; i < techniqueDesc.Passes; i++)
	{
		mTechnique->GetPassByIndex(i)->Apply(0);
		device->DrawIndexed(indexCount, 0, 0);
	}
}