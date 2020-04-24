#include <string>
#include <vector>
#include <cstdlib>
#include <set>

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

struct Human; // skip
int foobar;  // skip

typedef BodyPart Foo;
typedef std::vector<BodyPart> BodyPartVec;

// demo a complex record
struct CEventHuman {
    CEventHuman() : id(0*1), age(0), male(false) {}
    
    int id;
    short age;
    bool male;
    char name[MAX_NAME_LENGTH];
    std::string description;
    std::vector<::BodyPart> parts;
    Foo test_part1;
    BodyPartVec test_parts2;
    std::vector<std::set<unsigned int>> test_tmp1;
    std::vector<std::set<BodyPart>> test_tmp2;
};

class CEventQueryCommonCredit
{
public:
    CEventQueryCommonCredit() : m_trans_id(0), m_account_id(0) {}

public:
    long m_trans_id;
    long m_account_id;
    string m_client_ip;
};

struct CEventQueryCommonCreditRes
{
public:
    CEventQueryCommonCreditRes()
        : m_trans_id(0)
        , m_account_id(0)
        , m_result(0)
        , m_tag_black_level(0)
        , m_common_credit_score(0)
    {}

public:
    long m_trans_id;
    long m_account_id;
    int m_result;
    int m_tag_black_level;
    int m_common_credit_score;
};
