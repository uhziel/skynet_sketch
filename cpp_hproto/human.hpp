#include <string>
#include <vector>

#define MAX_NAME_LENGTH 32

enum BodyPartType
{
    Eye,
    Nose,
    Mouth,
};

struct BodyPart
{
    BodyPart() : type(0), weight(0) {}

    int type;
    float weight;
};

struct Human {
    Human() : id(0), age(0), male(false)
    {
        memset(name, 0, sizeof(name));
    }
    
    int id;
    short age;
    bool male;
    char name[MAX_NAME_LENGTH];
    std::string description;
    std::vector<BodyPart> parts;
};
