#include "matrix_graph.h"

class SecureAccess : MatrixGraph
{

public:
    //Terminal must always be the last item in this enum; it tells us its size
    enum roles {CUSTOMER, HR, DEVELOPER, ANALYST, ACCOUNTING, EXECUTIVE, TERMINAL};

    //Call this constructor AND parent constructor
    SecureAccess(int numUsers) : MatrixGraph(numUsers,TERMINAL+1){}
    bool EditAccess(int user, int group, bool newAccess)
    {
        return EditConnection(user, group, newAccess);
    }

    bool UserHasAccess(int user, int group)
    {
        return IsConnected(user, group);
    }

};