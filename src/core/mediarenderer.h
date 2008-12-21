#include "PltMediaRenderer.h"
#include "title.h"
#include "mediaplayer.h"
// #include "thread.h"

class Renderer : public PLT_MediaRenderer, MediaPlayer
{
public:
    Renderer(PlaybackCmdListener*      listener,
                      const char*          friendly_name,
                      bool                 show_ip = false,
                      const char*          uuid = NULL,
                      unsigned int         port = 0);

protected:
//     virtual NPT_Result OnNext(PLT_ActionReference& action);
    virtual NPT_Result OnPause(PLT_ActionReference& action);
    virtual NPT_Result OnPlay(PLT_ActionReference& action);
//     virtual NPT_Result OnPrevious(PLT_ActionReference& action);
//     virtual NPT_Result OnSeek(PLT_ActionReference& action);
    virtual NPT_Result OnStop(PLT_ActionReference& action);
    virtual NPT_Result OnSetAVTransportURI(PLT_ActionReference& action);
//     virtual NPT_Result OnSetPlayMode(PLT_ActionReference& action);

private:
    NPT_String m_currentUri;
    Title *m_currentTitle;
    bool   m_exposed;
};


// class RendererThread : public Thread
// {
// private:
//     virtual void run();
//     virtual bool suicide();
// };
