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

	QTimer *timer = new QTimer();
	timer->setSingleShot(true);
	timer->connect(timer, SIGNAL(timeout(QPrivateSignal)), this, SLOT(timeout(QPrivateSignal)));
	timer->start(5000);

	timerIdMap[timer] = id;
}

void BluemonkeyAgent::timeout(QPrivateSignal)
{
	quint64 id = timerIdMap[sender()];

	if(idList.contains(id))
	{
		DebugOut()<<"script aborted evaluation.  timed out."<<endl;
		engine()->abortEvaluation();
	}

	timerIdMap.remove(sender());

	sender()->deleteLater();
}

void BluemonkeyAgent::scriptUnload(qint64 id)
{
	idList.removeAll(id);
}
