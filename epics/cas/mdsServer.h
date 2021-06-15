#include <stdio.h>
#include <string.h>

#include "casdef.h"
#include "epicsAssert.h"
#include "epicsTime.h"
#include "gddAppFuncTable.h"
#include "resourceLib.h"
#include "smartGDDPointer.h"
#include "tsMinMax.h"

#ifndef NELEMENTS
#define NELEMENTS(A) (sizeof(A) / sizeof(A[0]))
#endif

#include <mdsobjects.h>
using namespace MDSplus;

class mdsPV;
class mdsServer;

//
// pvEntry
//
// entry in the string hash table for the pvInfo
//
// since there may be aliases then we may end up
// with several of this class all referencing
// the same pv info class
//
class pvEntry : public stringId, public tsSLNode<pvEntry>
{
public:
  pvEntry(mdsPV &pvIn, mdsServer &casIn, const char *pAliasName);
  ~pvEntry();
  void destroy();
  mdsPV *getPV();

private:
  mdsPV &pv;
  mdsServer &cas;
  pvEntry &operator=(const pvEntry &);
  pvEntry(const pvEntry &);
};

//
// mdsPV
//
class mdsPV : public casPV, public tsSLNode<mdsPV>
{
public:
  mdsPV(mdsServer &cas, char *name, Tree *tree, TreeNode *topNode, bool append);
  virtual ~mdsPV();

  // Called by the server libary each time that it wishes to
  // subscribe for PV the server tool via postEvent()
  caStatus interestRegister();

  // called by the server library each time that it wishes to
  // remove its subscription for PV value change events
  // from the server tool via caServerPostEvents()
  void interestDelete();

  aitEnum bestExternalType() const;

  caStatus read(const casCtx &, gdd &protoIn);
  caStatus write(const casCtx &, const gdd &value);
  void destroy();
  const char *getName() const;
  static void initFT();

  // chCreate() is called each time that a PV is attached to
  // by a client. The server tool must create a casChannel object
  // (or a derived class) each time that this routine is called
  //
  // If the operation must complete asynchronously then return
  // the status code S_casApp_asyncCompletion and then
  // create the casChannel object at some time in the future
  casChannel *createChannel(const casCtx &ctx, const char *const pUserName,
                            const char *const pHostName);

  // unsigned maxDimension() const;
  // aitIndex maxBound(unsigned dimension) const;
  bool isValid() { return valid; }
  void reference() { refCount++; }

protected:
  const gdd *currValue;
  mdsServer &cas;
  bool interest;
  bool preCreate;
  static epicsTime currentTime;
  Data *dataFromGdd(const gdd &inGdd);
  caStatus updateValue(const gdd &);

private:
  int refCount;
  bool append;
  bool isText;
  Tree *tree;
  TreeNode *valNode;
  char *name;
  int nDims;
  int *dims;

  aitFloat64 lopr, hopr, highAlarm, lowAlarm, highWarning, lowWarning, highCtrl,
      lowCtrl, highGraphic, lowGraphic;

  aitFloat64 defHighAlarm, defLowAlarm, defHighWarning, defLowWarning,
      defHighCtrl, defLowCtrl, defHighGraphic, defLowGraphic;

  int precision;
  char *units;
  char **enums;
  int numEnums;
  bool hasOpr, hasAlarm, hasWarning, hasCtrl, hasGraphic;
  Data *getDataFromGdd(gdd &in);
  void setGddFromData(Data *data, gdd &outGdd);

  // Std PV Attribute fetch support
  gddAppFuncTableStatus getPrecision(gdd &value);
  gddAppFuncTableStatus getHighAlarm(gdd &value);
  gddAppFuncTableStatus getLowAlarm(gdd &value);
  gddAppFuncTableStatus getHighWarning(gdd &value);
  gddAppFuncTableStatus getLowWarning(gdd &value);
  gddAppFuncTableStatus getHighCtrl(gdd &value);
  gddAppFuncTableStatus getLowCtrl(gdd &value);
  gddAppFuncTableStatus getHighGraphic(gdd &value);
  gddAppFuncTableStatus getLowGraphic(gdd &value);
  gddAppFuncTableStatus getUnits(gdd &value);
  gddAppFuncTableStatus getValue(gdd &value);
  gddAppFuncTableStatus getEnums(gdd &value);

  mdsPV &operator=(const mdsPV &);
  mdsPV(const mdsPV &);
  Data *getData();
  static gddAppFuncTable<mdsPV> ft;
  static char hasBeenInitialized;

  unsigned maxDimension() const;
  aitIndex maxBound(unsigned dimension) const;
  bool valid;
};

//
// mdsServer
//
class mdsServer : private caServer
{
public:
  mdsServer(Tree *tree, bool append);
  ~mdsServer();
  void removeAliasName(pvEntry &entry);
  void destroyAllPV();

private:
  resTable<pvEntry, stringId> stringResTbl;
  bool append;
  void installAliasName(mdsPV &info, const char *pAliasName);
  pvExistReturn pvExistTest(const casCtx &, const caNetAddr &,
                            const char *pPVName);
  pvExistReturn pvExistTest(const casCtx &, const char *pPVName);
  pvAttachReturn pvAttach(const casCtx &, const char *pPVName);

  mdsServer &operator=(const mdsServer &);
  mdsServer(const mdsServer &);
  char *makeUpper(const char *);
};

//
// mdsChannel
//
class mdsChannel : public casChannel
{
public:
  mdsChannel(const casCtx &ctxIn);
  void setOwner(const char *const pUserName, const char *const pHostName);
  bool readAccess() const;
  bool writeAccess() const;

private:
  mdsChannel &operator=(const mdsChannel &);
  mdsChannel(const mdsChannel &);
};

inline mdsChannel::mdsChannel(const casCtx &ctxIn) : casChannel(ctxIn) {}
