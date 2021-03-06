#pragma once
#include "Common/Core.h"
#include "Platform/Platform.h"
#include "Platform/GL/LoadOpenGL.h"


#include "Platform/GL/GLDebug.h"
#include "Platform/GlRenderer.h"
#include "Platform/AssetManager.h"

#include "Math/Math.h"
#include "Math/Color.h"
#include "Platform/UIContext.h"

#include "Platform/GLResources.h"
#include "Utility/Resource.h"
#include "Utility/DynamicArray.h"

#include "Components.h"
#include "Utility/DString.h"

#define ADD_COMPONENT(x) struct x { string name; }

static const v3 WhiteColorArray[4] = {
	{ 1.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f },
	{ 1.0f, 1.0f, 1.0f }
};

class App
{
private:
	enum pc_components
	{
		None = 0,
		Motherboard,
		Processor,
		Memory,
		Cooler,
		Storage,
		VideoCard,
		PowerSupply,
		Case,
		Total
	};
	a3::ui_context uiContext;
	a3::font_renderer fontRenderer;
	a3::basic2d_renderer renderer2D;

	a3::darray<casing> vCase;
	a3::darray<powerSupply> vPowerSupply;
	a3::darray<videoCard> vVideoCard;
	a3::darray<storage> vStorage;
	a3::darray<memory> vMemory;
	a3::darray<cpuCooler> vCpuCooler;
	a3::darray<motherboard> vMotherboard;
	a3::darray<cpu> vCpu;

	pc_components currentlyOpen;

	static const char* s_ComponentNames[pc_components::Total - 1];

public:
	App();
	void Init();
	void Update(const a3::input_info& input);
	void Render();

private:
	void SetComponents();
	void RenderUI(v2 position, v2 dimension);
	void RenderComponentWindow(pc_components c);
};

const char* App::s_ComponentNames[App::pc_components::Total - 1] =
{
	"Motherboard", "Processor", "Memory", "Cooler", "Storage", "Video Card", "Power Supply", "Case"
};

App::App() :
	uiContext(1280.0f, 720.0f),
	fontRenderer(a3::Renderer.CreateFontRenderer(a3::shaders::GLFontVertex, a3::shaders::GLFontFragment)),
	renderer2D(a3::Renderer.Create2DRenderer(a3::shaders::GLBasic2DVertex, a3::shaders::GLBasic2DFragment))
{
}

void App::Init()
{
	a3::Asset.LoadFontTextureAtlasFromFile(a3::asset_id::UserFont, "Resources/HackRegular.ttf", 100.0f);
	a3::Asset.LoadTexture2DFromFile(12, "Resources/BigSmile.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Motherboard, "Resources/MotherBoard.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Processor, "Resources/Processor.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Memory, "Resources/Memory.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Storage, "Resources/Storage.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Case, "Resources/Case.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::Cooler, "Resources/Cooler.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::PowerSupply, "Resources/PowerSupply.png", GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::Asset.LoadTexture2DFromFile(12 + pc_components::VideoCard, "Resources/VideoCard.png", GL_LINEAR, GL_CLAMP_TO_EDGE);

	currentlyOpen = pc_components::None;
	uiContext.SetColor(a3::color::Aqua, a3::color::Aqua, a3::color::Blurple, a3::color::WhiteSmoke);
	fontRenderer.SetRegion(0.0f, 1280.0f, 0.0f, 720.0f);
	fontRenderer.SetFont(a3::Asset.Get<a3::font_texture>(a3::asset_id::UserFont));

	renderer2D.SetRegion(0.0f, 1280.0f, 0.0f, 720.0f);

	SetComponents();

	//
	// TEST CODE
	//
	a3::Asset.LoadImageFromFile(51, "Resources/BigSmile100.png");
	a3::Asset.LoadImageFromFile(52, "Resources/BigSmile100.png");
	a3::Asset.LoadImageFromFile(53, "Resources/BigSmile100.png");
	a3::Asset.LoadImageFromFile(54, "Resources/BigSmile100.png");
	a3::Asset.LoadImageFromFile(55, "Resources/BigSmile100.png");
	a3::Asset.LoadImageFromFile(56, "Resources/BigSmile100.png");
	a3::Asset.LoadImageFromFile(57, "Resources/BigSmile100.png");
	a3::Asset.LoadImageFromFile(58, "Resources/BigSmile100.png");


	i32 ew = 100, eh = 100;
	i32 tot = 8;
	i32 w = ew * 2 + (10.0f / 100.0f * (f32)ew) * (2 + 1);
	i32 h = (tot / 2) * eh + (10.0f / 100.0f * (f32)eh) * (tot/2 + 1);
	a3::Asset.LoadTexture2DFromPixels(59, A3NULL, w, h, 4, GL_LINEAR, GL_CLAMP_TO_EDGE);
	a3::image_texture* t = a3::Asset.Get<a3::image_texture>(59);
	for (i32 y = 0; y < tot / 2; ++y)
	{
		i32 yoffset = eh * y + (y + 1)*(10.0f / 100.0f * eh);
		for (i32 x = 0; x < 2; ++x)
		{
			i32 xoffset = ew * x + (x + 1)*(10.0f / 100.0f * ew);
			a3::GLSubImageTexture2D(t, xoffset, yoffset, ew, eh, 4, a3::Asset.Get<a3::image>(51 + y * 2 + x)->Pixels);
		}
	}
	//a3::image* i = a3::Asset.Get<a3::image>(21);
	//a.wi
	
}

void App::Update(const a3::input_info& input)
{
	uiContext.UpdateIO(input);
}

void App::Render()
{
	fontRenderer.Render("PC Parts", { 1050.0f, 670.0f }, 45.0f, a3::color::White);
	RenderUI({ 1050.0f, 600.0f }, { 200.0f, 50.0f });
	RenderComponentWindow(currentlyOpen);

	if (uiContext.Button(55, { 350.0f, 400.0f }, 200.0f, 200.0f, a3::Asset.Get<a3::image_texture>(12 + currentlyOpen)))
	{
	}

	if (uiContext.Button(56, { 600.0f, 400.0f }, { 200.0f, 200.0f }, a3::Asset.Get<a3::image_texture>(12 + currentlyOpen)))
	{

	}

	renderer2D.BeginFrame();
	//renderer2D.Push({ 50.0f, 50.0f }, { 500.0f, 500.0f }, WhiteColorArray, a3::Asset.Get<a3::texture>(12 + currentlyOpen));
	//rect dest = { 0,0, 0, 0 };
	renderer2D.Push({ 50.0f, 50.0f }, 500.0f, a3::color::White, a3::Asset.Get<a3::image_texture>(12 + currentlyOpen));
	renderer2D.Push({ 0.0f, 500.0f }, { 500.0f, 500.0f }, a3::color::White, a3::Asset.Get<a3::image_texture>(12));
	renderer2D.EndFrame();
}

void App::SetComponents()
{
	vCase.Push(casing{ "Thermaltake Core G3","ATX Mid Tower",69.49 });
	vCase.Push(casing{ "Lian-Li PC-O10 WX","ATX Mid Tower",249.99 });

	vPowerSupply.Push(powerSupply{ "EVGA SuperNOVA 750","SuperNOVA G3","ATX","80+ Gold",750,"Full",4.5,99.89 });
	vPowerSupply.Push(powerSupply{ "Corsair CX550M","CXM","ATX","80+ Bronze",550,"Semi",4.5,33.99 });
	vPowerSupply.Push(powerSupply{ "Corsair TX650M Gold","TXM Gold","ATX","80+ Gold",650,"Semi",4.5,39.89 });

	vVideoCard.Push(videoCard{ "MSI GeForce GTX 1060 6GT OCV1","GT OCV1","GeForce GTX 1060 6GB",6,1.544,4.5, 529.89 });
	vVideoCard.Push(videoCard{ "Asus DUAL-GTX1070-O8G","Dual Series","GeForce GTX 1070",8,1.582,4.5, 439.89 });
	vVideoCard.Push(videoCard{ "Gigabyte GV-RX570AORUS-4GD","AORUS 4G","Radeon RX 570",4,1.168,4, 139.99 });

	vStorage.Push(storage{ "Western Digital WD10EZEX","Caviar Blue","3.5","7200 RPM","1 TB",64,4.5,44.89 });
	vStorage.Push(storage{ "Samsung MZ-76E500B/AM","860 Evo","2.5","SSD","500 GB",512,4,82.99 });
	vStorage.Push(storage{ "Intel SSDPECME040T401","DC P3608","PCI-E","SSD","4 TB",0,0,8867.99 });

	vMemory.Push(memory{ "Corsair Vengeance LPX","DDR4-3000","288-pin DIMM",15,2,8,16,4.5,114.99 });
	vMemory.Push(memory{ "G.Skill Aegis","DDR4-3000","288-pin DIMM",15,2,8,16,4.5,94.99 });
	vMemory.Push(memory{ "Team Vulcan","DDR4-3000","288-pin DIMM",15,2,4,8,4.5,60.88 });

	vCpuCooler.Push(cpuCooler{ "Cooler Master Hyper 212 EVO",600,2000,36,4.5,27.89 });
	vCpuCooler.Push(cpuCooler{ "Cooler Master Hyper T2",0,2800,35,4,16.89 });
	vCpuCooler.Push(cpuCooler{ "be quiet! Dark Rock 4",0,1400,35,4.5,68.99 });

	vCpu.Push(cpu{ "Intel Core i7 - 8700K",3.7,6,95,4.5,369.99 });
	vCpu.Push(cpu{ "Intel Core i9 - 9900K",3.6,8,95,4.5,529.99 });
	vCpu.Push(cpu{ "Intel Core i5 - 9600K",3.7,6,95,4.5,259.99 });

	vMotherboard.Push(motherboard{ "MSI Z370-A PRO","LGA1151","ATX",4,64,4,109.89 });
	vMotherboard.Push(motherboard{ "Asus PRIME Z390-A","LGA1151","ATX",4,64,0,183.00 });
	vMotherboard.Push(motherboard{ "Gigabyte H310M A","LGA1151","Micro ATX",2,32,5,55.99 });
};

void App::RenderUI(v2 position, v2 dimension)
{
	for (i32 i = 1; i < pc_components::Total; ++i)
	{
		if (uiContext.Button(i, position, dimension, s_ComponentNames[i - 1]))
		{
			if (currentlyOpen == (pc_components)i)
				currentlyOpen = pc_components::None;
			else
				currentlyOpen = (pc_components)i;
		}
		position.y -= dimension.y;
	}
}

void App::RenderComponentWindow(App::pc_components c)
{
	switch (c)
	{
	case pc_components::Motherboard:
	{
	



		break;
	}
	case pc_components::Processor:
	{

		break;
	}

	case pc_components::Memory:
	{

		break;
	}

	case pc_components::Cooler:
	{

		break;
	}

	case pc_components::Storage:
	{

		break;
	}

	case pc_components::VideoCard:
	{

		break;
	}

	case pc_components::PowerSupply:
	{

		break;
	}
	case pc_components::Case:
	{

		break;
	}
	}
}