#include <string>
#include <vector>
#include <cstdlib>

using namespace std;

#define MAX_NAME_LENGTH 32

enum BodyPartType
{
    Eye,
    Nose,
    Mouth,
};

struct BodyPart
{
    BodyPart() : type(2*3), weight(0.0f) {}

    int type;
    float weight;
};

struct Human;

typedef BodyPart Foo;

struct Human {
    Human() : id(0*1), age(0), male(false) {}
    
    int id;
    short age;
    bool male;
    char name[MAX_NAME_LENGTH];
    string description;
    vector<BodyPart> parts;
    //struct ::BodyPart part1;
    //Foo part1;
};
int foobar;