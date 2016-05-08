#include <Msvc\CthughaStd.h>

#include <CthughaEngine\CthughaApp.h>
#include <Graphics\Renderer.h>
#include <Graphics\Geometry.h>
#include <Graphics\MovementController.h>
#include <Graphics\SceneNodes.h>
#include <MainLoop\Initialization.h>
#include <EventManager\Events.h>
#include <EventManager\EventManagerImpl.h>
#include <Physics\PhysicsEventListener.h>
#include <UserInterface\HumanView.h>
#include <UserInterface\UserInterface.h>
#include <Entities\Entity.h>
#include <Entities\RenderComponent.h>
#include <Utilities\String.h>
#include <UserInterface\GUICreator.h>
#include "RVSDemoView.h"
#include "RVSDemoEvents.h"
#include <Graphics\GUI\UIFunctionText.h>
#include <Graphics\GUI\UIFunctionButton.h>
#include <Graphics\GUI\UIFunctionCheckBox.h>
#include <Graphics\SDFShadowManager.h>
#include <Graphics\ScreenPostProcessing.h>

#include <Graphics\Shader.h>

#include "RVSDemo.h"

extern RVSDemoApp g_EvolutioneApp;

#define CROSS_MULTIPLY_H(x, dim) (((x) / 1080.0f) * (dim))
#define CROSS_MULTIPLY_W(x, dim) (((x) / 1920.0f) * (dim))

//========================================================================
//
// MainMenuUI & MainMenuView implementation
//
//
//========================================================================

MainMenuView::MainMenuView(shared_ptr<Renderer> renderer) : HumanView(renderer, false)
{
	Vec2 vScreenSize = g_pApp->GetScreenSize();

	GUICreator::OnCreateImage(m_ViewId, "MainMenuBackground", "Art\\GUI\\MainMenuBackgroundImage.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec2(CROSS_MULTIPLY_W(1080, vScreenSize.x), CROSS_MULTIPLY_H(1080, vScreenSize.y)), g_pApp->GetScreenSize(), Vec2(1.0f, 1.0f), 0);
	std::shared_ptr<UINode> pTextNode = GUICreator::OnCreateText(m_ViewId, "ProgramVersionText", Vec2(CROSS_MULTIPLY_W(50, vScreenSize.x), CROSS_MULTIPLY_H(100, vScreenSize.y)), Vec2(250, 50), Vec2(1.0f, 1.0f), "font\\Arial.fnt", 30, 0.1f, "LEFT_ALIGN", "TOP_ALIGN", Vec4(0.75f, 0.75f, 0.75f, 1.0f), 0);

	std::shared_ptr<UIFunctionText> pFunctionText = std::static_pointer_cast<UIFunctionText>(pTextNode->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("LOADING SCENE AND COMPILING SHADERS ...");
}

MainMenuView::~MainMenuView()
{
}


void MainMenuView::VRenderText()
{
	HumanView::VRenderText();
}


void MainMenuView::VOnUpdate(unsigned long deltaMs)
{
	HumanView::VOnUpdate( deltaMs );

	g_pApp->m_Options.m_expectedPlayers = 1;
	g_pApp->m_Options.m_Level = "maps\\RVSDemo.xml";

	shared_ptr<EvtData_Request_Start_Game> pRequestStartGameEvent(CHG_NEW EvtData_Request_Start_Game());
	IEventManager::Get()->VQueueEvent(pRequestStartGameEvent);
}

bool MainMenuView::VOnMsgProc( CHG_Event msg )
{
	if (HumanView::VOnMsgProc(msg))
		return 1;
	
	return 0;
}

//========================================================================
//
// EvolutioneHumanView Implementation
//
//========================================================================

#define MAX_SHADOW_ELONGATION 5.0f

AncientCivilizationDemoHumanView::AncientCivilizationDemoHumanView(shared_ptr<Renderer> renderer) : HumanView(renderer)
{ 
    m_bPauseState = false;
    RegisterAllDelegates();

	Vec2 vScreenSize = g_pApp->GetScreenSize();

	//Camera Init Position ---------------------------------------------------------->Change
	m_pCamera->VSetTransform(&Mat4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 10.0f, 0.0f, 0.0f, 0.0f, 1.0f));

	m_pPauseMenuDialog = GUICreator::OnCreateImage(m_ViewId, "PauseMenuDialog", "Art\\GUI\\BubblePauseMenu.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec2(), Vec2(CROSS_MULTIPLY_W(656.0f, vScreenSize.x), CROSS_MULTIPLY_H(1080.0f, vScreenSize.y)), Vec2(1.0f, 1.0f), 0);
    m_pPauseMenuDialog->SetVisible(m_bPauseState);

    m_pQuitButtonDialog = GUICreator::OnCreateButton(m_ViewId, "QuitButtonDialog", "Art\\GUI\\BubbleBottonImage.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		Vec2(CROSS_MULTIPLY_W(20, vScreenSize.x), CROSS_MULTIPLY_H(950, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(120, vScreenSize.x), CROSS_MULTIPLY_H(40, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"font\\Arial.fnt", (int)CROSS_MULTIPLY_H(30, vScreenSize.y), 0.1f, "CENTER_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f),
        true, Vec4(1.0f, 1.0f, 1.0f, 0.0f), Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

    m_pQuitButtonDialog->SetVisible(false);
    std::shared_ptr<UIFunctionText> pFunctionQuitButtonText = std::static_pointer_cast<UIFunctionText>(m_pQuitButtonDialog->GetUIFunction(UIFUNCTION_TEXT));
    pFunctionQuitButtonText->SetText("Quit");
	std::shared_ptr<UIFunctionButton> pFunctionExitButtonButton = std::static_pointer_cast<UIFunctionButton>(m_pQuitButtonDialog->GetUIFunction(UIFUNTION_BUTTON));
	pFunctionExitButtonButton->SetActionPerformed(OnActionPerformedQuitButtonDialog, this);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pSsaoCheckBox = GUICreator::OnCreateCheckBox(m_ViewId, "SsaoCheckBox", Vec2(CROSS_MULTIPLY_W(66, vScreenSize.x), CROSS_MULTIPLY_H(88, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(27, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pSsaoCheckBox->SetVisible(false);
	std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pSsaoCheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedSsaoCheckBox, this);

	m_pBloomCheckBox = GUICreator::OnCreateCheckBox(m_ViewId, "BloomCheckBox", Vec2(CROSS_MULTIPLY_W(66, vScreenSize.x), CROSS_MULTIPLY_H(134, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(27, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pBloomCheckBox->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pBloomCheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedBloomCheckBox, this);

	m_pGICheckBox = GUICreator::OnCreateCheckBox(m_ViewId, "GICheckBox", Vec2(CROSS_MULTIPLY_W(66, vScreenSize.x), CROSS_MULTIPLY_H(180, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(27, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pGICheckBox->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pGICheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedGICheckBox, this);

	m_pVoxelDebugCheckBox = GUICreator::OnCreateCheckBox(m_ViewId, "VoxelDebugCheckBox", Vec2(CROSS_MULTIPLY_W(80, vScreenSize.x), CROSS_MULTIPLY_H(222, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(20, vScreenSize.x), CROSS_MULTIPLY_H(20, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pVoxelDebugCheckBox->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pVoxelDebugCheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedVoxelDebugCheckBox, this);

	m_pTwoBounceGICheckBox = GUICreator::OnCreateCheckBox(m_ViewId, "TwoBounceGI", Vec2(CROSS_MULTIPLY_W(80, vScreenSize.x), CROSS_MULTIPLY_H(250, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(20, vScreenSize.x), CROSS_MULTIPLY_H(20, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pTwoBounceGICheckBox->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pTwoBounceGICheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedTwoBouncesGICheckBox, this);

	m_pDirectionalLightCheckBox1 = GUICreator::OnCreateCheckBox(m_ViewId, "DirectionalLightCheckBox1", Vec2(CROSS_MULTIPLY_W(66, vScreenSize.x), CROSS_MULTIPLY_H(330, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(27, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pDirectionalLightCheckBox1->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pDirectionalLightCheckBox1->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedDirectionalLightCheckBox1, this);

	m_pDirectionalLightCheckBox2 = GUICreator::OnCreateCheckBox(m_ViewId, "DirectionalLightCheckBox2", Vec2(CROSS_MULTIPLY_W(66, vScreenSize.x), CROSS_MULTIPLY_H(381, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(27, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pDirectionalLightCheckBox2->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pDirectionalLightCheckBox2->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedDirectionalLightCheckBox2, this);

	m_pDirectionalLightCheckBox3 = GUICreator::OnCreateCheckBox(m_ViewId, "DirectionalLightCheckBox3", Vec2(CROSS_MULTIPLY_W(312, vScreenSize.x), CROSS_MULTIPLY_H(330, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(27, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pDirectionalLightCheckBox3->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pDirectionalLightCheckBox3->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedDirectionalLightCheckBox3, this);

	m_pDirectionalLightCheckBox4 = GUICreator::OnCreateCheckBox(m_ViewId, "DirectionalLightCheckBox4", Vec2(CROSS_MULTIPLY_W(312, vScreenSize.x), CROSS_MULTIPLY_H(381, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(27, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pDirectionalLightCheckBox4->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pDirectionalLightCheckBox4->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedDirectionalLightCheckBox4, this);

	m_pPointLightCheckBox1 = GUICreator::OnCreateCheckBox(m_ViewId, "PointLightCheckBox1", Vec2(CROSS_MULTIPLY_W(66, vScreenSize.x), CROSS_MULTIPLY_H(432, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(27, vScreenSize.y)), Vec2(1.0f, 1.0f),
		"Art\\GUI\\CheckBoxBackground.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		"Art\\GUI\\CheckBoxCheck.dds", Vec4(1.0f, 1.0f, 1.0f, 1.0f),
		true, Vec4(1.0f, 1.0f, 1.0f, 1.0f), Vec4(0.7f, 0.7f, 0.7f, 1.0f), Vec4(1.0f, 0.5f, 0.5f, 1.0f), Vec4(), 5.0f, 0);

	m_pPointLightCheckBox1->SetVisible(false);
	pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pPointLightCheckBox1->GetUIFunction(UIFUNCTION_CHECKBOX));
	pFunctionCheckBox->SetActive(true);
	pFunctionCheckBox->SetActionPerformed(OnActionPerformedPointLightCheckBox1, this);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	m_pBloomCheckBoxText = GUICreator::OnCreateText(m_ViewId, "BloomCheckBoxText", Vec2(CROSS_MULTIPLY_W(108, vScreenSize.x), CROSS_MULTIPLY_H(0, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(24, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	std::shared_ptr<UIFunctionText> pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pBloomCheckBoxText->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Ssao");

	m_pBloomCheckBoxText->SetVisible(false);

	m_pSsaoCheckBoxText = GUICreator::OnCreateText(m_ViewId, "SsaoCheckBoxText", Vec2(CROSS_MULTIPLY_W(108, vScreenSize.x), CROSS_MULTIPLY_H(46, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(24, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pSsaoCheckBoxText->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Bloom");

	m_pSsaoCheckBoxText->SetVisible(false);

	m_pGICheckBoxText = GUICreator::OnCreateText(m_ViewId, "GICheckBoxText", Vec2(CROSS_MULTIPLY_W(108, vScreenSize.x), CROSS_MULTIPLY_H(92, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(24, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pGICheckBoxText->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Global Illumination");

	m_pGICheckBoxText->SetVisible(false);

	m_pVoxelDebugCheckBoxText = GUICreator::OnCreateText(m_ViewId, "VoxelDebugCheckBoxText", Vec2(CROSS_MULTIPLY_W(115, vScreenSize.x), CROSS_MULTIPLY_H(132, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(18, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pVoxelDebugCheckBoxText->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Show voxel");

	m_pVoxelDebugCheckBoxText->SetVisible(false);

	m_pTwoBounceGICheckBoxText = GUICreator::OnCreateText(m_ViewId, "TwoBounceGICheckBoxText", Vec2(CROSS_MULTIPLY_W(115, vScreenSize.x), CROSS_MULTIPLY_H(160, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(18, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pTwoBounceGICheckBoxText->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("GI Two Bounce");

	m_pTwoBounceGICheckBoxText->SetVisible(false);

	m_pDirectionalLightCheckBoxText1 = GUICreator::OnCreateText(m_ViewId, "DirectionalLightCheckBoxText1", Vec2(CROSS_MULTIPLY_W(108, vScreenSize.x), CROSS_MULTIPLY_H(242, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(24, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pDirectionalLightCheckBoxText1->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Point Light 1");

	m_pDirectionalLightCheckBoxText1->SetVisible(false);

	m_pDirectionalLightCheckBoxText2 = GUICreator::OnCreateText(m_ViewId, "DirectionalLightCheckBoxText2", Vec2(CROSS_MULTIPLY_W(108, vScreenSize.x), CROSS_MULTIPLY_H(293, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(24, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pDirectionalLightCheckBoxText2->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Point Light 2");

	m_pDirectionalLightCheckBoxText2->SetVisible(false);

	m_pDirectionalLightCheckBoxText3 = GUICreator::OnCreateText(m_ViewId, "DirectionalLightCheckBoxText3", Vec2(CROSS_MULTIPLY_W(352, vScreenSize.x), CROSS_MULTIPLY_H(242, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(24, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pDirectionalLightCheckBoxText3->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Point Light 3");

	m_pDirectionalLightCheckBoxText3->SetVisible(false);

	m_pDirectionalLightCheckBoxText4 = GUICreator::OnCreateText(m_ViewId, "DirectionalLightCheckBoxText4", Vec2(CROSS_MULTIPLY_W(352, vScreenSize.x), CROSS_MULTIPLY_H(293, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(24, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pDirectionalLightCheckBoxText4->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Point Light 4");

	m_pDirectionalLightCheckBoxText4->SetVisible(false);

	m_pPointLightCheckBoxText1 = GUICreator::OnCreateText(m_ViewId, "PointLightCheckBoxText1", Vec2(CROSS_MULTIPLY_W(108, vScreenSize.x), CROSS_MULTIPLY_H(344, vScreenSize.y)), Vec2(CROSS_MULTIPLY_W(27, vScreenSize.x), CROSS_MULTIPLY_H(200, vScreenSize.y)),
		Vec2(1.0f, 1.0f), "font\\Arial.fnt", (int)CROSS_MULTIPLY_H(24, vScreenSize.y), 0.1f, "LEFT_ALIGN", "MIDDLE_ALIGN", Vec4(0.0f, 0.0f, 0.0f, 1.0f), 0);

	pFunctionText = std::static_pointer_cast<UIFunctionText>(m_pPointLightCheckBoxText1->GetUIFunction(UIFUNCTION_TEXT));
	pFunctionText->SetText("Spot Light 1");

	m_pPointLightCheckBoxText1->SetVisible(false);

	g_pApp->SetMouseMode(CHG_RELATIVE_MODE);
}


AncientCivilizationDemoHumanView::~AncientCivilizationDemoHumanView()
{
    RemoveAllDelegates();
}

void AncientCivilizationDemoHumanView::OnActionPerformedQuitButtonDialog(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			g_pApp->SetQuitting(true);

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedSsaoCheckBox(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pSsaoCheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));

			pObject->m_pScene->GetScreenPostProcessing()->SsaoActive(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedBloomCheckBox(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pBloomCheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));

			pObject->m_pScene->GetScreenPostProcessing()->BloomActive(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedGICheckBox(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pGICheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBoxVoxelDebug = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pVoxelDebugCheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBoxTwoBounces = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pTwoBounceGICheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
					
			g_pApp->m_pVoxelManager->setActiveGI(pFunctionCheckBox->IsActive());
			pObject->m_pVoxelDebugCheckBox->SetVisible(pFunctionCheckBox->IsActive());
			pObject->m_pVoxelDebugCheckBoxText->SetVisible(pFunctionCheckBox->IsActive());
			pObject->m_pTwoBounceGICheckBox->SetVisible(pFunctionCheckBox->IsActive());
			pObject->m_pTwoBounceGICheckBoxText->SetVisible(pFunctionCheckBox->IsActive());
			pFunctionCheckBoxVoxelDebug->SetActive(false);
			pFunctionCheckBoxTwoBounces->SetActive(true);

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedTwoBouncesGICheckBox(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pTwoBounceGICheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));

			g_pApp->m_pVoxelManager->SetActiveTwoBounces(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedVoxelDebugCheckBox(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pVoxelDebugCheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));

			g_pApp->m_pVoxelManager->ActiveDebugMode(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedDirectionalLightCheckBox1(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pDirectionalLightCheckBox1->GetUIFunction(UIFUNCTION_CHECKBOX));
			
			std::shared_ptr<Entity> pDirectionalLightEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_0"));
			std::shared_ptr<ISceneNode> pDirectionalLightNode = pObject->m_pScene->FindEntity(pDirectionalLightEntity->GetId());

			std::shared_ptr<Entity> pPointLightSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere_0"));
			std::shared_ptr<ISceneNode> pPointLightSphereNode = pObject->m_pScene->FindEntity(pPointLightSphereEntity->GetId());

			pDirectionalLightNode->VSetVisible(pFunctionCheckBox->IsActive());
			pPointLightSphereNode->VSetVisible(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedDirectionalLightCheckBox2(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pDirectionalLightCheckBox2->GetUIFunction(UIFUNCTION_CHECKBOX));
			
			std::shared_ptr<Entity> pDirectionalLightEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_1"));
			std::shared_ptr<ISceneNode> pDirectionalLightNode = pObject->m_pScene->FindEntity(pDirectionalLightEntity->GetId());

			std::shared_ptr<Entity> pPointLightSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere_1"));
			std::shared_ptr<ISceneNode> pPointLightSphereNode = pObject->m_pScene->FindEntity(pPointLightSphereEntity->GetId());

			pDirectionalLightNode->VSetVisible(pFunctionCheckBox->IsActive());
			pPointLightSphereNode->VSetVisible(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedDirectionalLightCheckBox3(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pDirectionalLightCheckBox3->GetUIFunction(UIFUNCTION_CHECKBOX));
			
			std::shared_ptr<Entity> pDirectionalLightEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_2"));
			std::shared_ptr<ISceneNode> pDirectionalLightNode = pObject->m_pScene->FindEntity(pDirectionalLightEntity->GetId());

			std::shared_ptr<Entity> pPointLightSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere_2"));
			std::shared_ptr<ISceneNode> pPointLightSphereNode = pObject->m_pScene->FindEntity(pPointLightSphereEntity->GetId());

			pDirectionalLightNode->VSetVisible(pFunctionCheckBox->IsActive());
			pPointLightSphereNode->VSetVisible(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedDirectionalLightCheckBox4(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pDirectionalLightCheckBox4->GetUIFunction(UIFUNCTION_CHECKBOX));
			
			std::shared_ptr<Entity> pDirectionalLightEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLight_3"));
			std::shared_ptr<ISceneNode> pDirectionalLightNode = pObject->m_pScene->FindEntity(pDirectionalLightEntity->GetId());

			std::shared_ptr<Entity> pPointLightSphereEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("PointLightSphere_3"));
			std::shared_ptr<ISceneNode> pPointLightSphereNode = pObject->m_pScene->FindEntity(pPointLightSphereEntity->GetId());

			pDirectionalLightNode->VSetVisible(pFunctionCheckBox->IsActive());
			pPointLightSphereNode->VSetVisible(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

void AncientCivilizationDemoHumanView::OnActionPerformedPointLightCheckBox1(CHG_Event msg, void* pUserContext)
{
	switch (msg.eventType)
	{
		case CHG_MOUSEBUTTONUP:
		{
			AncientCivilizationDemoHumanView* pObject = (AncientCivilizationDemoHumanView*)pUserContext;
			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(pObject->m_pPointLightCheckBox1->GetUIFunction(UIFUNCTION_CHECKBOX));
			
			std::shared_ptr<Entity> pPointLightEntity = MakeStrongPtr(g_pApp->m_pGame->VGetEntity("SpotLight_0"));
			std::shared_ptr<ISceneNode> pPointLightNode = pObject->m_pScene->FindEntity(pPointLightEntity->GetId());

			pPointLightNode->VSetVisible(pFunctionCheckBox->IsActive());

			break;
		}
	}
}

//
// TeapotWarsHumanView::VOnMsgProc				- Chapter 19, page 727
//
bool AncientCivilizationDemoHumanView::VOnMsgProc(CHG_Event msg)
{
	if (HumanView::VOnMsgProc(msg))
		return 1;

	if (msg.eventType==CHG_KEYDOWN)
	{
        if (msg.keyboardEvent.asciiCode == '\x1b')
        {
            m_bPauseState = !m_bPauseState;

			std::shared_ptr<UIFunctionCheckBox> pFunctionCheckBox = std::static_pointer_cast<UIFunctionCheckBox>(m_pGICheckBox->GetUIFunction(UIFUNCTION_CHECKBOX));
            
            //m_pEscMenuBackGround->SetVisible(m_bPauseState);
            m_pPauseMenuDialog->SetVisible(m_bPauseState);
            m_pQuitButtonDialog->SetVisible(m_bPauseState);

			m_pSsaoCheckBox->SetVisible(m_bPauseState);
			m_pBloomCheckBox->SetVisible(m_bPauseState);
			m_pGICheckBox->SetVisible(m_bPauseState);
			m_pVoxelDebugCheckBox->SetVisible(pFunctionCheckBox->IsActive() && m_bPauseState);
			m_pTwoBounceGICheckBox->SetVisible(pFunctionCheckBox->IsActive() && m_bPauseState);
			m_pDirectionalLightCheckBox1->SetVisible(m_bPauseState);
			m_pDirectionalLightCheckBox2->SetVisible(m_bPauseState);
			m_pDirectionalLightCheckBox3->SetVisible(m_bPauseState);
			m_pDirectionalLightCheckBox4->SetVisible(m_bPauseState);
			m_pPointLightCheckBox1->SetVisible(m_bPauseState);

			m_pSsaoCheckBoxText->SetVisible(m_bPauseState);
			m_pBloomCheckBoxText->SetVisible(m_bPauseState);
			m_pGICheckBoxText->SetVisible(m_bPauseState);
			m_pVoxelDebugCheckBoxText->SetVisible(pFunctionCheckBox->IsActive() && m_bPauseState);
			m_pTwoBounceGICheckBoxText->SetVisible(pFunctionCheckBox->IsActive() && m_bPauseState);
			m_pDirectionalLightCheckBoxText1->SetVisible(m_bPauseState);
			m_pDirectionalLightCheckBoxText2->SetVisible(m_bPauseState);
			m_pDirectionalLightCheckBoxText3->SetVisible(m_bPauseState);
			m_pDirectionalLightCheckBoxText4->SetVisible(m_bPauseState);
			m_pPointLightCheckBoxText1->SetVisible(m_bPauseState);
			
			if (m_bPauseState)
				g_pApp->SetMouseMode(CHG_ABSOLUTE_MODE);
			else
				g_pApp->SetMouseMode(CHG_RELATIVE_MODE);

			TogglePause(m_bPauseState);
        }
	}

	return 0;
}


//
// EvolutioneHumanView::VRenderText				
//
void AncientCivilizationDemoHumanView::VRenderText()
{
    char bufferText[100];
    memset(bufferText, 0, 4);

	static float timeCount = 0.0f;
	static float fps = 1.0f;
	static int ticks = 0;

	if(timeCount > 2.0f)
	{
		fps = (float)ticks/timeCount;
		timeCount = 0.0f;
		ticks = 0;

		//printf("FPS: %f\n", fps);
	}

    sprintf(bufferText, "%f", fps);

	timeCount += g_pApp->getElapsedTime();
	ticks++;

    //g_pApp->m_pTextHelper->SetColor( Color(1.0f, 1.0f, 1.0f, 1.0f) );

    g_pApp->m_pRenderer->setViewport((int)g_pApp->GetScreenSize().x, (int)g_pApp->GetScreenSize().y);
	g_pApp->m_pRenderer->changeToMainFramebuffer();
}



//
// TeapotWarsHumanView::VOnUpdate				- Chapter 19, page 730
//
void AncientCivilizationDemoHumanView::VOnUpdate(unsigned long deltaMs)
{
	HumanView::VOnUpdate( deltaMs );

	if (!m_bPause)
	{	
		if (m_pFreeCameraController)
		{
			m_pFreeCameraController->OnUpdate(deltaMs);
		}

		if (m_pPlayerController)
		{
			m_pPlayerController->OnUpdate(deltaMs);
		}
	}

	//Send out a tick to listeners.
	shared_ptr<EvtData_Update_Tick> pTickEvent(CHG_NEW EvtData_Update_Tick(deltaMs));
    IEventManager::Get()->VTriggerEvent(pTickEvent);
}

//
// TeapotWarsHumanView::VOnAttach				- Chapter 19, page 731
//
void AncientCivilizationDemoHumanView::VOnAttach(GameViewId vid, EntityId aid)
{
	HumanView::VOnAttach(vid, aid);
}

bool AncientCivilizationDemoHumanView::VLoadGameDelegate(TiXmlElement* pLevelData)
{
	if (!HumanView::VLoadGameDelegate(pLevelData))
		return false;

    // A movement controller is going to control the camera, 
    m_pFreeCameraController.reset(CHG_NEW MovementController(m_pCamera, 0, 0, false));

    //-------------------------------------------------------------> Change Place
    m_KeyboardHandler = m_pFreeCameraController;
	m_PointerHandler = m_pFreeCameraController;

	m_pScene->BuildOctree();
    m_pScene->VOnRestore();
    return true;
}

void AncientCivilizationDemoHumanView::VSetControlledActor(EntityId entityId)
{ 
	m_pPlayer = MakeStrongPtr(g_pApp->m_pGame->VGetEntity(entityId));
    if (!m_pPlayer)
    {
        CHG_ERROR("Invalid Player");
        return;
    }

	HumanView::VSetControlledActor(entityId);

    m_pPlayerController.reset(CHG_NEW KinematicPlayerController(m_pPlayer, m_pCamera));
    m_KeyboardHandler = m_pPlayerController;
    m_PointerHandler = m_pPlayerController;

	m_pFreeCameraController.reset();
}

void AncientCivilizationDemoHumanView::GameplayUiUpdateDelegate(IEventDataPtr pEventData)
{
}

void AncientCivilizationDemoHumanView::SetControlledActorDelegate(IEventDataPtr pEventData)
{
    shared_ptr<EvtData_Set_Controlled_Actor> pCastEventData = static_pointer_cast<EvtData_Set_Controlled_Actor>(pEventData);
    VSetControlledActor(pCastEventData->GetActorId());
}

void AncientCivilizationDemoHumanView::RegisterAllDelegates(void)
{
	// [mrmike] Move, New, and Destroy actor events are now handled by the HumanView, as are the PlaySound and GameState events

    IEventManager* pGlobalEventManager = IEventManager::Get();
    pGlobalEventManager->VAddListener(MakeDelegate(this, &AncientCivilizationDemoHumanView::SetControlledActorDelegate), EvtData_Set_Controlled_Actor::sk_EventType);
}

void AncientCivilizationDemoHumanView::RemoveAllDelegates(void)
{
    IEventManager* pGlobalEventManager = IEventManager::Get();
    pGlobalEventManager->VRemoveListener(MakeDelegate(this, &AncientCivilizationDemoHumanView::SetControlledActorDelegate), EvtData_Set_Controlled_Actor::sk_EventType);
}