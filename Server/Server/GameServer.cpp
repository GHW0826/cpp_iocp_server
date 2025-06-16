#include "pch.h"
#include "ThreadManager.h"
#include "Service.h"
#include "HttpSession.h"
#include "GameSession.h"
#include "iniparser.h"
#include "TickRunner.h"

enum {
	WORKER_TICK = 64,
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true) {
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		service->GetIocpCore()->Dispatch(10);

		// 예약된 일감 처리 (Timer)
		ThreadManager::DistributeReservedJobs();

		// 글로벌 큐 로직
		ThreadManager::DoGlobalQueueWork();
	}
}

int main()
{
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IOCPCore>(),
		MakeShared<HttpSession>,
		100);
	/*
	ServerServiceRef service = MakeShared<ServerService>(
		NetAddress(L"127.0.0.1", 7777),
		MakeShared<IOCPCore>(),
		MakeShared<GameSession>,
		100);
	*/

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; ++i) {
		GThreadManager->Launch([&service]() {
			DoWorkerJob(service);
		});
	}

	for (int32 i = 0; i < 1; ++i) {
		GThreadManager->Launch([]() {
		});
	}

	DoWorkerJob(service);
	
	GThreadManager->Join();

	return 0;
}
