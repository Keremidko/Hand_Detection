
class Debugger {
private:
	static Debugger* _instance;
	Debugger();
public:
	static Debugger* getInstance();
};