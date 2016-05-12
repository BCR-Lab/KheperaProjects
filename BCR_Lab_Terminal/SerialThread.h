#include <wx/wxprec.h>

class SerialThread : public wxThread
{
public:
	SerialThread();
	virtual ~SerialThread();

	// thread execution starts here
	virtual void *Entry();

public:
	unsigned m_count;
};

/*
class SerialWorkerThread : public wxThread
{
public:
	SerialWorkerThread(GUIFrame *frame);

	// thread execution starts here
	virtual void *Entry() ;

	// called when the thread exits - whether it terminates normally or is
	// stopped with Delete() (but not when it is Kill()ed!)
	virtual void OnExit() ;

public:
	GUIFrame *m_frame;
	unsigned m_count;
};*/