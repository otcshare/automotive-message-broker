#include "agent.h"
#include <debugout.h>
#include <QTimer>
#include <QScriptEngine>

BluemonkeyAgent::BluemonkeyAgent(QScriptEngine *engine)
	:QScriptEngineAgent(engine)
{
}

void BluemonkeyAgent::scriptLoad(qint64 id, const QString &program, const QString &fileName, int baseLineNumber)
{
	DebugOut()<<"executing script"<<endl;

	QTimer* timer = new QTimer(this);

	timer->setInterval(20000);
	timer->setSingleShot(true);
	timer->start();

	QScriptEngine* e = engine();

	connect(timer, &QTimer::timeout, [id,&e,&timer,this](){
		if(idList.contains(id))
		{
			e->abortEvaluation();
		}
	});
}

void BluemonkeyAgent::scriptUnload(qint64 id)
{
	idList.removeAll(id);
}
