#include "setupHelpers.h"
#include "signalHelpers.h"
#include <cassert>
#include <Eris/PollDefault.h>
#include <Eris/Entity.h>
#include <sigc++/object_slot.h>
#include <iostream>

AutoConnection stdConnect()
{
    AutoConnection con(new Eris::Connection("eris-test", "localhost", 7450, false));
    SignalCounter0 connectCount;
    con->Connected.connect(SigC::slot(connectCount, &SignalCounter0::fired));
    
    SignalCounter1<const std::string&> fail;
    con->Failure.connect(SigC::slot(fail, &SignalCounter1<const std::string&>::fired));
    
    con->connect();
    
    while (!connectCount.fireCount() && !fail.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(con->isConnected());
    return con;
}

AutoAccount stdLogin(const std::string& uname, const std::string& pwd, Eris::Connection* con)
{
    AutoAccount player(new Eris::Account(con));
    
    SignalCounter0 loginCount;
    player->LoginSuccess.connect(SigC::slot(loginCount, &SignalCounter0::fired));
   
    SignalCounter1<const std::string&> loginErrorCounter;
    player->LoginFailure.connect(SigC::slot(loginErrorCounter, &SignalCounter1<const std::string&>::fired));
    
    player->login(uname, pwd);

    while (!loginCount.fireCount() && !loginErrorCounter.fireCount())
    {
        Eris::PollDefault::poll();
    }
    
    assert(loginErrorCounter.fireCount() == 0);
    return player;
}

AvatarGetter::AvatarGetter(Eris::Account* acc) : 
    m_acc(acc),
    m_expectFail(false),
    m_failed(false)
{
    m_acc->AvatarSuccess.connect(SigC::slot(*this, &AvatarGetter::success));
    m_acc->AvatarFailure.connect(SigC::slot(*this, &AvatarGetter::failure));
}
    
AutoAvatar AvatarGetter::take(const std::string& charId)
{        
    m_waiting = true;
    m_failed = false;
    
    m_acc->takeCharacter(charId);
    
    while (m_waiting) Eris::PollDefault::poll();
    
    // we wanted to fail, bail out now
    if (m_failed && m_expectFail) return AutoAvatar();
    
    assert(m_av->getEntity() == NULL); // shouldn't have the entity yet
    assert(m_av->getId() == charId); // but should have it's ID

    SignalCounter1<Eris::Entity*> gotChar;
    m_av->GotCharacterEntity.connect(SigC::slot(gotChar, &SignalCounter1<Eris::Entity*>::fired));

    while (gotChar.fireCount() == 0) Eris::PollDefault::poll();

    assert(m_av->getEntity());
    assert(m_av->getEntity()->getId() == charId);

    return m_av;
}
    
void AvatarGetter::success(Eris::Avatar* av)
{
    m_av.reset(av);
    m_waiting = false;
}

void AvatarGetter::failure(const std::string& msg)
{
    if (msg != "deliberate") {
        std::cerr << "failure getting an avatar: " << msg << std::endl;
    }
    m_waiting = false;
    m_failed = true;
}