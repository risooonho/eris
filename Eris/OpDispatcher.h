#ifndef ERIS_OP_DISPATCH_H
#define ERIS_OP_DISPATCH_H

#include <Eris/Dispatcher.h>

namespace Eris
{

class OpFromDispatcher : public StdBranchDispatcher
{
public:	
	OpFromDispatcher(const std::string &nm, const std::string &id) :
		StdBranchDispatcher(nm), _id(id) 
	{;}
	
	virtual ~OpFromDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _id;
};
	

class OpToDispatcher : public StdBranchDispatcher
{
public:	
	OpToDispatcher(const std::string &nm, const std::string &id) :
		StdBranchDispatcher(nm), _id(id)
	{;}
	
	virtual ~OpToDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const std::string _id;
};


class OpRefnoDispatcher : public StdBranchDispatcher
{
public:	
	OpRefnoDispatcher(const std::string &nm, long refno, unsigned depth = 0) :
		StdBranchDispatcher(nm), _refno(refno), _depth(depth)
	{;}
	
	virtual ~OpRefnoDispatcher() {;}
		
	virtual bool dispatch(DispatchContextDeque &dq);
protected:
	const long _refno;
	const unsigned _depth;
};

}

#endif