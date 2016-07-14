// This is free and unencumbered software released into the public domain.
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non-commercial, and by any
// means.
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// For more information, please refer to <http://unlicense.org/>

#define MICROPROFILE_HELP_ALT "Right-Click"
#define MICROPROFILE_HELP_MOD "Ctrl"

#ifdef _WIN32
#include <stdio.h>
#include <stdarg.h>

#include <windows.h>
void uprintf(const char* fmt, ...)
{
	char buffer[32*1024];
	va_list args;
	va_start (args, fmt);
	vsprintf_s(buffer, fmt, args);
	OutputDebugString(&buffer[0]);
	va_end (args);
}
//change printf function to dump in debugger
#define MICROPROFILE_PRINTF uprintf
#endif

//#define MICROPROFILE_WEBSERVER 0
#define MICROPROFILEUI_IMPL
#include "microprofile.h"
#include "microprofileui.h"

#include "glinc.h"

#if MICROPROFILE_ENABLED
struct MicroProfileVertex
{
	float nX;
	float nY;
	uint32_t nColor;
	float fU;
	float fV;
};


#define MICROPROFILE_MAX_VERTICES (16<<10)
#define MAX_FONT_CHARS 256
#define Q0(d, member, v) d[0].member = v
#define Q1(d, member, v) d[1].member = v; d[3].member = v
#define Q2(d, member, v) d[4].member = v
#define Q3(d, member, v) d[2].member = v; d[5].member = v

void MicroProfileFlush();
namespace
{
	uint32_t nVertexPos = 0;
	MicroProfileVertex nDrawBuffer[MICROPROFILE_MAX_VERTICES];
	enum
	{
		MAX_COMMANDS = 1024,
	};
	struct 
	{
		uint32_t nCommand;
		uint32_t nNumVertices;
	} DrawCommands[MAX_COMMANDS];

	int32_t nNumDrawCommands;
	uint32_t g_nW;
	uint32_t g_nH;
	uint32_t g_VAO;
	uint32_t g_VertexBuffer;
	int 	g_LocVertexIn;
	int 	g_LocColorIn;
	int 	g_LocTC0In;
	int 	g_LocTex;
	int 	g_LocProjectionMatrix;
	int 	g_LocfRcpFontHeight;

	float g_Projection[16];


	GLuint g_VertexShader;
	GLuint g_PixelShader;
	GLuint g_Program;
	GLuint g_FontTexture;


	struct SFontDescription
	{
		uint16_t nCharOffsets[MAX_FONT_CHARS];
		uint32_t nTextureId;
	};

	extern unsigned char g_Font[]; 
	SFontDescription g_FontDescription;


	const char* g_PixelShaderCode = "\
#version 150 \n \
uniform sampler2D tex; \
uniform float fRcpFontHeight; \
in vec2 TC0; \
in vec4 Color; \
out vec4 Out0; \
 \
void main(void)   \
{   \
	vec4 color = texture(tex, TC0.xy); \
	if(TC0.x > 1.0 ) \
	{ \
		Out0.xyz = Color.xyz; \
		Out0.w = Color.w;	 \
	} \
	else \
	{ \
		vec4 c1 = texture(tex, TC0.xy + vec2(0.0, fRcpFontHeight));\
		Out0 = color * Color; \
		if(color.w < 0.5){ \
			Out0 = vec4(0,0,0,c1.w);\
		}\
	} \
} \
";
	const char* g_VertexShaderCode = " \
#version 150 \n \
uniform mat4 ProjectionMatrix; \
in vec3 VertexIn; \
in vec4 ColorIn; \
in vec2 TC0In; \
out vec2 TC0; \
out vec4 Color; \
 \
void main(void)   \
{ \
	Color = ColorIn; \
	TC0 = TC0In; \
	gl_Position = ProjectionMatrix * vec4(VertexIn, 1.0); \
} \
";



	MicroProfileVertex* PushVertices(uint32_t nCommand, int nCount)
	{
		if(nVertexPos + nCount > MICROPROFILE_MAX_VERTICES)
		{
			MicroProfileFlush();
		}
		MP_ASSERT(nVertexPos + nCount <= MICROPROFILE_MAX_VERTICES);

		uint32_t nOut = nVertexPos;
		nVertexPos += nCount;

		if(nNumDrawCommands && DrawCommands[nNumDrawCommands-1].nCommand == nCommand)
		{
			DrawCommands[nNumDrawCommands-1].nNumVertices += nCount;
		}
		else
		{
			MP_ASSERT(nNumDrawCommands < MAX_COMMANDS);
			DrawCommands[nNumDrawCommands].nCommand = nCommand;
			DrawCommands[nNumDrawCommands].nNumVertices = nCount;
			++nNumDrawCommands;
		}
		return &nDrawBuffer[nOut];
	}
	void DumpGlLog(GLuint handle)
	{
		int nLogLen = 0;
		return;//temp hack
		MP_ASSERT(0 == glGetError());
		glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &nLogLen);
		if(nLogLen > 1) 
		{
			char* pChars = (char*)malloc(nLogLen);
			int nLen = 0;
			glGetInfoLogARB(handle, nLogLen, &nLen, pChars);

			printf("COMPILE MESSAGE\n%s\n\n", pChars);

			free(pChars);
			MP_BREAK();
		}
		MP_ASSERT(0 == glGetError());
	}

	GLuint CreateProgram(int nType, const char* pShader)
	{
		MP_ASSERT(0 == glGetError());
		GLuint handle = glCreateShaderObjectARB(nType);
		glShaderSource(handle, 1, (const char**)&pShader, 0);
		glCompileShader(handle);
		DumpGlLog(handle);
		MP_ASSERT(handle);	
		MP_ASSERT(0 == glGetError());
		return handle;
	}
}

#define FONT_TEX_X 1024
#define FONT_TEX_Y 9
#define UNPACKED_SIZE (FONT_TEX_X*FONT_TEX_Y * 4)


void MicroProfileDrawInit()
{
	glGenBuffers(1, &g_VertexBuffer);
	glGenVertexArrays(1, &g_VAO);
	g_PixelShader = CreateProgram(GL_FRAGMENT_SHADER_ARB, g_PixelShaderCode);
	g_VertexShader = CreateProgram(GL_VERTEX_SHADER_ARB, g_VertexShaderCode);
	g_Program = glCreateProgramObjectARB();
	glAttachObjectARB(g_Program, g_PixelShader);
	glAttachObjectARB(g_Program, g_VertexShader);

	g_LocVertexIn = 1;
	g_LocColorIn = 2;
	g_LocTC0In = 3;

	glBindAttribLocation(g_Program, g_LocColorIn, "ColorIn");
	glBindAttribLocation(g_Program, g_LocVertexIn, "VertexIn");
	glBindAttribLocation(g_Program, g_LocTC0In, "TC0In");
	glLinkProgramARB(g_Program);
	DumpGlLog(g_Program);

	g_LocTex = glGetUniformLocation(g_Program, "tex");
	g_LocProjectionMatrix = glGetUniformLocation(g_Program, "ProjectionMatrix");
	g_LocfRcpFontHeight = glGetUniformLocation(g_Program, "fRcpFontHeight");

	for(uint32_t i = 0; i < MAX_FONT_CHARS; ++i)
	{
		g_FontDescription.nCharOffsets[i] = 206;
	}
	for(uint32_t i = 'A'; i <= 'Z'; ++i)
	{
		g_FontDescription.nCharOffsets[i] = (i-'A')*8+1;
	}
	for(uint32_t i = 'a'; i <= 'z'; ++i)
	{
		g_FontDescription.nCharOffsets[i] = (i-'a')*8+217;
	}
	for(uint32_t i = '0'; i <= '9'; ++i)
	{
		g_FontDescription.nCharOffsets[i] = (i-'0')*8+433;
	}
	for(uint32_t i = '!'; i <= '/'; ++i)
	{
		g_FontDescription.nCharOffsets[i] = (i-'!')*8+513;
	}
	for(uint32_t i = ':'; i <= '@'; ++i)
	{
		g_FontDescription.nCharOffsets[i] = (i-':')*8+625+8;
	}
	for(uint32_t i = '['; i <= '_'; ++i)
	{
		g_FontDescription.nCharOffsets[i] = (i-'[')*8+681+8;
	}
	for(uint32_t i = '{'; i <= '~'; ++i)
	{
		g_FontDescription.nCharOffsets[i] = (i-'{')*8+721+8;
	}

	uint32_t* pUnpacked = (uint32_t*)alloca(UNPACKED_SIZE);
	int idx = 0;
	int end = FONT_TEX_X * FONT_TEX_Y / 8;
	for(int i = 0; i < end; i++)
	{
		unsigned char pValue = g_Font[i];
		for(int j = 0; j < 8; ++j)
		{
			pUnpacked[idx++] = pValue & 0x80 ? (uint32_t)-1 : 0;
			pValue <<= 1;
		}
	}
	uint32_t* p4 = &pUnpacked[0];
	glGenTextures(1, &g_FontTexture);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);
	{
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);     
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, FONT_TEX_X, FONT_TEX_Y, 0, GL_RGBA, GL_UNSIGNED_BYTE, &p4[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void MicroProfileBeginDraw(uint32_t nWidth, uint32_t nHeight, float* prj)
{
	memcpy(g_Projection, prj, sizeof(float)*16);
	g_nW = nWidth;
	g_nH = nHeight;
	nVertexPos = 0;
	nNumDrawCommands = 0;
}
void MicroProfileFlush()
{
	if(0 == nVertexPos)
		return;
	MICROPROFILE_SCOPEI("MicroProfile", "Flush", 0xffff3456);
	MICROPROFILE_SCOPEGPUI("FlushDraw", 0xffff4444);



	glUseProgramObjectARB(g_Program);


	glUniform1i(g_LocTex, 0);
	glUniformMatrix4fv(g_LocProjectionMatrix, 1, 0, g_Projection);
	glUniform1f(g_LocfRcpFontHeight, 1.0f / FONT_TEX_Y);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_FontTexture);


	glBindVertexArray(g_VAO);


	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(nDrawBuffer), &nDrawBuffer[0], GL_STREAM_DRAW);
	int nStride = sizeof(MicroProfileVertex);
	glVertexAttribPointer(g_LocVertexIn, 2, GL_FLOAT, 0 , nStride, 0);
	glVertexAttribPointer(g_LocColorIn, 4, GL_UNSIGNED_BYTE, GL_TRUE, nStride, (void*)(offsetof(MicroProfileVertex, nColor)));
	glVertexAttribPointer(g_LocTC0In, 2, GL_FLOAT, 0, nStride, (void*)(offsetof(MicroProfileVertex, fU)));
	glEnableVertexAttribArray(g_LocVertexIn);
	glEnableVertexAttribArray(g_LocColorIn);
	glEnableVertexAttribArray(g_LocTC0In);

	int nOffset = 0;
	for(int i = 0; i < nNumDrawCommands; ++i)
	{
		int nCount = DrawCommands[i].nNumVertices;
		glDrawArrays(DrawCommands[i].nCommand, nOffset, nCount);
		nOffset += nCount;
	}
	glDisableVertexAttribArray(g_LocVertexIn);
	glDisableVertexAttribArray(g_LocColorIn);
	glDisableVertexAttribArray(g_LocTC0In);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgramObjectARB(0);
	glBindVertexArray(0);
	nVertexPos = 0;
	nNumDrawCommands = 0;
}
void MicroProfileEndDraw()
{
	MicroProfileFlush();

}


#if MICROPROFILEUI_ENABLED
void MicroProfileDrawText(int nX, int nY, uint32_t nColor, const char* pText, uint32_t nLen)
{
	MICROPROFILE_SCOPEI("MicroProfile", "TextDraw", 0xff88ee);
	const float fOffsetU = 5.f / 1024.f;
	MP_ASSERT(nLen <= strlen(pText));
	float fX = (float)nX;
	float fY = (float)nY;
	float fY2 = fY + (MICROPROFILE_TEXT_HEIGHT+1);

	MicroProfileVertex* pVertex = PushVertices(GL_TRIANGLES, 6 * nLen);
	const char* pStr = pText;
	nColor = 0xff000000|((nColor&0xff)<<16)|(nColor&0xff00)|((nColor>>16)&0xff);

	for(uint32_t j = 0; j < nLen; ++j)
	{
		int16_t nOffset = g_FontDescription.nCharOffsets[(int)*pStr++];
		float fOffset = nOffset / 1024.f;
		Q0(pVertex,nX, fX);
		Q0(pVertex,nY, fY);
		Q0(pVertex,nColor, nColor);
		Q0(pVertex,fU, fOffset);
		Q0(pVertex,fV, 0.f);
		
		Q1(pVertex, nX, fX+MICROPROFILE_TEXT_WIDTH);
		Q1(pVertex, nY, fY);
		Q1(pVertex, nColor, nColor);
		Q1(pVertex, fU, fOffset+fOffsetU);
		Q1(pVertex, fV, 0.f);

		Q2(pVertex, nX, fX+MICROPROFILE_TEXT_WIDTH);
		Q2(pVertex, nY, fY2);
		Q2(pVertex, nColor, nColor);
		Q2(pVertex, fU, fOffset+fOffsetU);
		Q2(pVertex, fV, 1.f);


		Q3(pVertex, nX, fX);
		Q3(pVertex, nY, fY2);
		Q3(pVertex, nColor, nColor);
		Q3(pVertex, fU, fOffset);
		Q3(pVertex, fV, 1.f);

		fX += MICROPROFILE_TEXT_WIDTH+1;
		pVertex += 6;
	}
}
void MicroProfileDrawBox(int nX0, int nY0, int nX1, int nY1, uint32_t nColor, MicroProfileBoxType Type)
{
	if(Type == MicroProfileBoxTypeFlat)
	{
		nColor = ((nColor&0xff)<<16)|((nColor>>16)&0xff)|(0xff00ff00&nColor);
		MicroProfileVertex* pVertex = PushVertices(GL_TRIANGLES, 6);
		Q0(pVertex, nX, (float)nX0);
		Q0(pVertex, nY, (float)nY0);
		Q0(pVertex, nColor, nColor);
		Q0(pVertex, fU, 2.f);
		Q0(pVertex, fV, 2.f);
		Q1(pVertex, nX, (float)nX1);
		Q1(pVertex, nY, (float)nY0);
		Q1(pVertex, nColor, nColor);
		Q1(pVertex, fU, 2.f);
		Q1(pVertex, fV, 2.f);
		Q2(pVertex, nX, (float)nX1);
		Q2(pVertex, nY, (float)nY1);
		Q2(pVertex, nColor, nColor);
		Q2(pVertex, fU, 2.f);
		Q2(pVertex, fV, 2.f);
		Q3(pVertex, nX, (float)nX0);
		Q3(pVertex, nY, (float)nY1);
		Q3(pVertex, nColor, nColor);
		Q3(pVertex, fU, 2.f);
		Q3(pVertex, fV, 2.f);
	}
	else
	{
		uint32_t r = 0xff & (nColor>>16);
		uint32_t g = 0xff & (nColor>>8);
		uint32_t b = 0xff & nColor;
		uint32_t nMax = MicroProfileMax(MicroProfileMax(MicroProfileMax(r, g), b), 30u);
		uint32_t nMin = MicroProfileMin(MicroProfileMin(MicroProfileMin(r, g), b), 180u);

		uint32_t r0 = 0xff & ((r + nMax)/2);
		uint32_t g0 = 0xff & ((g + nMax)/2);
		uint32_t b0 = 0xff & ((b + nMax)/2);

		uint32_t r1 = 0xff & ((r+nMin)/2);
		uint32_t g1 = 0xff & ((g+nMin)/2);
		uint32_t b1 = 0xff & ((b+nMin)/2);
		uint32_t nColor0 = (r0<<0)|(g0<<8)|(b0<<16)|(0xff000000&nColor);
		uint32_t nColor1 = (r1<<0)|(g1<<8)|(b1<<16)|(0xff000000&nColor);
		MicroProfileVertex* pVertex = PushVertices(GL_TRIANGLES, 6);
		Q0(pVertex, nX, (float)nX0);
		Q0(pVertex, nY, (float)nY0);
		Q0(pVertex, nColor, nColor0);
		Q0(pVertex, fU, 2.f);
		Q0(pVertex, fV, 2.f);
		Q1(pVertex, nX, (float)nX1);
		Q1(pVertex, nY, (float)nY0);
		Q1(pVertex, nColor, nColor0);
		Q1(pVertex, fU, 3.f);
		Q1(pVertex, fV, 2.f);
		Q2(pVertex, nX, (float)nX1);
		Q2(pVertex, nY, (float)nY1);
		Q2(pVertex, nColor, nColor1);
		Q2(pVertex, fU, 3.f);
		Q2(pVertex, fV, 3.f);
		Q3(pVertex, nX, (float)nX0);
		Q3(pVertex, nY, (float)nY1);
		Q3(pVertex, nColor, nColor1);
		Q3(pVertex, fU, 2.f);
		Q3(pVertex, fV, 3.f);
	}
}


void MicroProfileDrawLine2D(uint32_t nVertices, float* pVertices, uint32_t nColor)
{
	if(!nVertices) return;

	MicroProfileVertex* pVertex = PushVertices(GL_LINES, 2*(nVertices-1));
	nColor = ((nColor&0xff)<<16)|(nColor&0xff00ff00)|((nColor>>16)&0xff);
	for(uint32_t i = 0; i < nVertices-1; ++i)
	{
		pVertex[0].nX = pVertices[i*2];
		pVertex[0].nY = pVertices[i*2+1] ;
		pVertex[0].nColor = nColor;
		pVertex[0].fU = 2.f;
		pVertex[0].fV = 2.f;
		pVertex[1].nX = pVertices[(i+1)*2];
		pVertex[1].nY = pVertices[(i+1)*2+1] ;
		pVertex[1].nColor = nColor;
		pVertex[1].fU = 2.f;
		pVertex[1].fV = 2.f;
		pVertex += 2;
	}
}
#endif

namespace
{
	unsigned char g_Font[] = 
	{
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x10,0x78,0x38,0x78,0x7c,0x7c,0x3c,0x44,0x38,0x04,0x44,0x40,0x44,0x44,0x38,0x78,
		0x38,0x78,0x38,0x7c,0x44,0x44,0x44,0x44,0x44,0x7c,0x00,0x00,0x40,0x00,0x04,0x00,
		0x18,0x00,0x40,0x10,0x08,0x40,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x38,0x10,0x38,0x7c,0x08,0x7c,0x1c,0x7c,0x38,0x38,
		0x10,0x28,0x28,0x10,0x00,0x20,0x10,0x08,0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x04,0x00,0x20,0x38,0x38,0x70,0x00,0x1c,0x10,0x00,0x1c,0x10,0x70,0x30,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x28,0x44,0x44,0x44,0x40,0x40,0x40,0x44,0x10,0x04,0x48,0x40,0x6c,0x44,0x44,0x44,
		0x44,0x44,0x44,0x10,0x44,0x44,0x44,0x44,0x44,0x04,0x00,0x00,0x40,0x00,0x04,0x00,
		0x24,0x00,0x40,0x00,0x00,0x40,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x30,0x44,0x04,0x18,0x40,0x20,0x04,0x44,0x44,
		0x10,0x28,0x28,0x3c,0x44,0x50,0x10,0x10,0x08,0x54,0x10,0x00,0x00,0x00,0x04,0x00,
		0x00,0x08,0x00,0x10,0x44,0x44,0x40,0x40,0x04,0x28,0x00,0x30,0x10,0x18,0x58,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x44,0x44,0x40,0x44,0x40,0x40,0x40,0x44,0x10,0x04,0x50,0x40,0x54,0x64,0x44,0x44,
		0x44,0x44,0x40,0x10,0x44,0x44,0x44,0x28,0x28,0x08,0x00,0x38,0x78,0x3c,0x3c,0x38,
		0x20,0x38,0x78,0x30,0x18,0x44,0x10,0x6c,0x78,0x38,0x78,0x3c,0x5c,0x3c,0x3c,0x44,
		0x44,0x44,0x44,0x44,0x7c,0x00,0x4c,0x10,0x04,0x08,0x28,0x78,0x40,0x08,0x44,0x44,
		0x10,0x00,0x7c,0x50,0x08,0x50,0x00,0x20,0x04,0x38,0x10,0x00,0x00,0x00,0x08,0x10,
		0x10,0x10,0x7c,0x08,0x08,0x54,0x40,0x20,0x04,0x44,0x00,0x30,0x10,0x18,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x44,0x78,0x40,0x44,0x78,0x78,0x40,0x7c,0x10,0x04,0x60,0x40,0x54,0x54,0x44,0x78,
		0x44,0x78,0x38,0x10,0x44,0x44,0x54,0x10,0x10,0x10,0x00,0x04,0x44,0x40,0x44,0x44,
		0x78,0x44,0x44,0x10,0x08,0x48,0x10,0x54,0x44,0x44,0x44,0x44,0x60,0x40,0x10,0x44,
		0x44,0x44,0x28,0x44,0x08,0x00,0x54,0x10,0x18,0x18,0x48,0x04,0x78,0x10,0x38,0x3c,
		0x10,0x00,0x28,0x38,0x10,0x20,0x00,0x20,0x04,0x10,0x7c,0x00,0x7c,0x00,0x10,0x00,
		0x00,0x20,0x00,0x04,0x10,0x5c,0x40,0x10,0x04,0x00,0x00,0x60,0x10,0x0c,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x7c,0x44,0x40,0x44,0x40,0x40,0x4c,0x44,0x10,0x04,0x50,0x40,0x44,0x4c,0x44,0x40,
		0x54,0x50,0x04,0x10,0x44,0x44,0x54,0x28,0x10,0x20,0x00,0x3c,0x44,0x40,0x44,0x7c,
		0x20,0x44,0x44,0x10,0x08,0x70,0x10,0x54,0x44,0x44,0x44,0x44,0x40,0x38,0x10,0x44,
		0x44,0x54,0x10,0x44,0x10,0x00,0x64,0x10,0x20,0x04,0x7c,0x04,0x44,0x20,0x44,0x04,
		0x10,0x00,0x7c,0x14,0x20,0x54,0x00,0x20,0x04,0x38,0x10,0x10,0x00,0x00,0x20,0x10,
		0x10,0x10,0x7c,0x08,0x10,0x58,0x40,0x08,0x04,0x00,0x00,0x30,0x10,0x18,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x44,0x44,0x44,0x44,0x40,0x40,0x44,0x44,0x10,0x44,0x48,0x40,0x44,0x44,0x44,0x40,
		0x48,0x48,0x44,0x10,0x44,0x28,0x6c,0x44,0x10,0x40,0x00,0x44,0x44,0x40,0x44,0x40,
		0x20,0x3c,0x44,0x10,0x08,0x48,0x10,0x54,0x44,0x44,0x44,0x44,0x40,0x04,0x12,0x4c,
		0x28,0x54,0x28,0x3c,0x20,0x00,0x44,0x10,0x40,0x44,0x08,0x44,0x44,0x20,0x44,0x08,
		0x00,0x00,0x28,0x78,0x44,0x48,0x00,0x10,0x08,0x54,0x10,0x10,0x00,0x00,0x40,0x00,
		0x10,0x08,0x00,0x10,0x00,0x40,0x40,0x04,0x04,0x00,0x00,0x30,0x10,0x18,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x44,0x78,0x38,0x78,0x7c,0x40,0x3c,0x44,0x38,0x38,0x44,0x7c,0x44,0x44,0x38,0x40,
		0x34,0x44,0x38,0x10,0x38,0x10,0x44,0x44,0x10,0x7c,0x00,0x3c,0x78,0x3c,0x3c,0x3c,
		0x20,0x04,0x44,0x38,0x48,0x44,0x38,0x44,0x44,0x38,0x78,0x3c,0x40,0x78,0x0c,0x34,
		0x10,0x6c,0x44,0x04,0x7c,0x00,0x38,0x38,0x7c,0x38,0x08,0x38,0x38,0x20,0x38,0x70,
		0x10,0x00,0x28,0x10,0x00,0x34,0x00,0x08,0x10,0x10,0x00,0x20,0x00,0x10,0x00,0x00,
		0x20,0x04,0x00,0x20,0x10,0x3c,0x70,0x00,0x1c,0x00,0x7c,0x1c,0x10,0x70,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x38,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x40,0x04,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	};

}
#endif