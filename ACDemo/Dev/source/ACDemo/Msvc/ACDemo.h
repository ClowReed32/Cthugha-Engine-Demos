class AncientCivilizationDemoApp : public CthughaApp
{
protected:
	virtual BaseGameLogic *VCreateGameAndView();

public:
	virtual char *VGetGameTitle() { return "Ancient_Civilization_Demo"; }
};

class CreatureEditor;

//---------------------------------------------------------------------------------------------------------------------
// EvolutioneLogic class                        
//---------------------------------------------------------------------------------------------------------------------
class AncientCivilizationDemoLogic : public BaseGameLogic
{
public:
	AncientCivilizationDemoLogic();
	virtual ~AncientCivilizationDemoLogic();

	// Evolutione Methods

	// Update
    //virtual void VMoveActor(const EntityId id, Mat4x4 const &mat);

	// Overloads
	virtual void VChangeState(BaseGameState newState);
	virtual void VAddView(shared_ptr<IGameView> pView, EntityId entityId=INVALID_ENTITY_ID);
	virtual EntityFactory* VCreateEntityFactory(void) override;
	virtual void VOnUpdate(float time, float elapsedTime);
	
    // event delegates
	void RequestStartGameDelegate(IEventDataPtr pEventData);
	void EnvironmentLoadedDelegate(IEventDataPtr pEventData);
	void StartThrustDelegate(IEventDataPtr pEventData);
    void EndThrustDelegate(IEventDataPtr pEventData);
    void StartSteerDelegate(IEventDataPtr pEventData);
    void EndSteerDelegate(IEventDataPtr pEventData);
	void StartJumpDelegate(IEventDataPtr pEventData);

protected:
    virtual bool VLoadGameDelegate(TiXmlElement* pLevelData);

private:
    void RegisterAllDelegates(void);
    void RemoveAllDelegates(void);
};
