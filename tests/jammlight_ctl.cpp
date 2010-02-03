// -------------------------------------------------------------------
// DeviceNameCtrl.h

class DeviceNameController : public ControllerImplAdapter
{
public:
// sync interface
    void Action(const in1&, out1&);
    
// getter for StateVars
// -> query for unevented StateVars
// -> read cached value for evented StateVars
    T _getStateVar();
    
protected:
// async interface
    void _reqAction(const in1&);
    virtual void _ansAction(const in1&, out1&) = 0;
    
// value of evented StateVar changed
    virtual void _changedStateVar(const T&) = 0;
    
// device added
    virtual void _newDevice(DeviceRoot*) = 0;
    
// device removed
    virtual void _remDevice(DeviceRoot*) = 0;
};


// -------------------------------------------------------------------
// DeviceNameCtrl.cpp

void DeviceNameController::Action(const in1&, out1&)
{
        // create action and set arguments
        // action -> xml
        // post action message to be send via http-session
        // receive response
        // xml -> action
        // get action out arguments and set arguments out1
}

// call as active method in seperate thread
void DeviceNameController::_reqAction(const in1&)
{
    outType1 out1;
        // more outVars ...
    Action(in1, out1);
    
    _ansAction(in1, out1)
}


// -------------------------------------------------------------------
// DeviceNameCtrlImpl.h.sample

class DeviceNameCtrlImpl
{
private:
// async interface
    virtual void _ansAction(const in1&, out1&);
    
// value of evented StateVar changed
    virtual void _changedStateVar(const T&);
    
// device added
    virtual void _newDevice(DeviceRoot*);
    
// device removed
    virtual void _remDevice(DeviceRoot*);
};


// -------------------------------------------------------------------
// DeviceNameCtrlImpl.cpp.sample

void DeviceNameCtrlImpl::_ansAction(const in1&, out1&)
{
}

void DeviceNameCtrlImpl::_changedStateVar(const T&)
{
}

void DeviceNameCtrlImpl::_newDevice(DeviceRoot*)
{
}

void DeviceNameCtrlImpl::_remDevice(DeviceRoot*)
{
}
