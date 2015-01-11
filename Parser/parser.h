
class Variables
{

public:
	static Variables* Instance();
	
private:
	Variables();
	static Variables* instance;
};