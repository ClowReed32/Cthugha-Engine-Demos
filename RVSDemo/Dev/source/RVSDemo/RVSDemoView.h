#include <Graphics\SceneNodes.h>
#include "..\..\EventManager\EventManager.h"
#include <UserInterface\HumanView.h>
#include <UserInterface\UserInterface.h>
#include <Graphics\MovementController.h>

#include "KinematicPlayerController.h"

struct EditorViewResource
{
	std::string sResourceName;
	int iUsed;
	float fUnusedTime;
};

class MainMenuView : public HumanView
{
protected:

public:

    MainMenuView(shared_ptr<Renderer> renderer);
	~MainMenuView(); 
	virtual bool VOnMsgProc( CHG_Event msg );
	virtual void VRenderText();	
	virtual void VOnUpdate(unsigned long deltaMs);
};

class AncientCivilizationDemoHumanView : public HumanView
{
protected:
	bool  m_bShowUI;					// If true, it renders the UI control text
    std::wstring m_gameplayText;

	shared_ptr<KinematicPlayerController> m_pPlayerController;
	shared_ptr<MovementController> m_pFreeCameraController;
	shared_ptr<Entity> m_pPlayer;

    // Pause
    bool m_bPauseState;

    // GUI Components
    std::shared_ptr<UINode> m_pEscMenuBackGround;
    std::shared_ptr<UINode> m_pPauseMenuDialog;
    std::shared_ptr<UINode> m_pQuitButtonDialog;

	std::shared_ptr<UINode> m_pSsaoCheckBox;
	std::shared_ptr<UINode> m_pBloomCheckBox;
	std::shared_ptr<UINode> m_pGICheckBox;
	std::shared_ptr<UINode> m_pVoxelDebugCheckBox;
	std::shared_ptr<UINode> m_pTwoBounceGICheckBox;
	std::shared_ptr<UINode> m_pDirectionalLightCheckBox1;
	std::shared_ptr<UINode> m_pDirectionalLightCheckBox2;
	std::shared_ptr<UINode> m_pDirectionalLightCheckBox3;
	std::shared_ptr<UINode> m_pDirectionalLightCheckBox4;
	std::shared_ptr<UINode> m_pPointLightCheckBox1;

	std::shared_ptr<UINode> m_pSsaoCheckBoxText;
	std::shared_ptr<UINode> m_pBloomCheckBoxText;
	std::shared_ptr<UINode> m_pGICheckBoxText;
	std::shared_ptr<UINode> m_pVoxelDebugCheckBoxText;
	std::shared_ptr<UINode> m_pTwoBounceGICheckBoxText;
	std::shared_ptr<UINode> m_pDirectionalLightCheckBoxText1;
	std::shared_ptr<UINode> m_pDirectionalLightCheckBoxText2;
	std::shared_ptr<UINode> m_pDirectionalLightCheckBoxText3;
	std::shared_ptr<UINode> m_pDirectionalLightCheckBoxText4;
	std::shared_ptr<UINode> m_pPointLightCheckBoxText1;

 
public:
	AncientCivilizationDemoHumanView(shared_ptr<Renderer> renderer);
	virtual ~AncientCivilizationDemoHumanView();

	virtual bool VOnMsgProc( CHG_Event msg );	
	virtual void VRenderText();	
	virtual void VOnUpdate(unsigned long deltaMs);
	virtual void VOnAttach(GameViewId vid, EntityId aid);

	virtual void VSetControlledActor(EntityId entityId);
	virtual bool VLoadGameDelegate(TiXmlElement* pLevelData) override;

    // event delegates
    void GameplayUiUpdateDelegate(IEventDataPtr pEventData);
    void SetControlledActorDelegate(IEventDataPtr pEventData);

	// UI methods
	static void OnActionPerformedQuitButtonDialog(CHG_Event msg, void* pUserContext);

	static void OnActionPerformedSsaoCheckBox(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedBloomCheckBox(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedGICheckBox(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedVoxelDebugCheckBox(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedDirectionalLightCheckBox1(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedDirectionalLightCheckBox2(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedDirectionalLightCheckBox3(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedDirectionalLightCheckBox4(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedPointLightCheckBox1(CHG_Event msg, void* pUserContext);
	static void OnActionPerformedTwoBouncesGICheckBox(CHG_Event msg, void* pUserContext);

private:
    void RegisterAllDelegates(void);
    void RemoveAllDelegates(void);
};