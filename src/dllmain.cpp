#include "dllmain.h"

// It is recommended that you use multi-threaded asynchronous processing when developing plug-ins
// as to minimize the impact of the plug-in on the performance of the game itself.

bool                    alive          = true;
std::thread *           mainLoop       = nullptr;
std::mutex              mainLoopMutex;
std::condition_variable mainLoopCv;

std::queue<std::string> messageQueue = {};

bool cardStatus = false;
bool qrStatus   = false;

patches::Plugins::CommitCardCallback    commitCard    = nullptr;
patches::Plugins::CommitQrCallback      commitQr      = nullptr;
patches::Plugins::CommitQrLoginCallback commitQrLogin = nullptr;

void MainLoop() {
    std::unique_lock<std::mutex> mainLoopLock(mainLoopMutex);
    // Do in-thread initialize job
    std::cout << "Hello Plugin Thread!" << std::endl;
    while (alive) {
        // Do in-thread update things
        while (!messageQueue.empty ()) {
            std::string message = messageQueue.front ();
            messageQueue.pop ();

            std::cout << "Plugin Thread says: " << message << std::endl;
        }

        if (alive) mainLoopCv.wait (mainLoopLock);
        // if (alive) mainLoopCv.wait_for (mainLoopLock, std::chrono::milliseconds (500));      // if you want this thread auto awake
    }
    // Do in-thread destrpy job
    std::cout << "Goodbye Plugin Thread!" << std::endl;
}

extern "C" {
    // Life cycle API
    void Init () {
        // Called at the beginning of the game, do some simple initialization, don't take too long
        std::cout << "Hello Plugin!" << std::endl;
        mainLoop = new std::thread (MainLoop);
        mainLoop->detach ();
    }

    void Update () {
        // It is called continuously during the game running, so it should not take too long.
        // Not suggest to do things here, if you want to do some thing at update, you should make a new thread and sync processing by condition_variable
        // std::cout << "Do not log here, just do your work! Otherwise Console will spamming!" << std::endl;
    }

    void Exit () {
        // Called at Exit button is pressed, you have a little time to do destroy job, don't take too long.
        std::cout << "Goodbye Plugin!" << std::endl;
        alive = false;
        mainLoopCv.notify_all ();
    }

    // New API
    void InitVersion (uint64_t gameVersion) {
        // Called at game got version of current Taiko.exe
        // Refer to https://github.com/esuo1198/TaikoArcadeLoader/blob/Refactor/src/constants.h
        // UNKNOWN = 0x0000000000000000  game should terminate before you got this
        // JPN00   = 0x4C07355966D815FB
        // JPN08   = 0x67C0F3042746D488
        // JPN39   = 0x49F643ADB6B18705
        // CHN00   = 0xA7EE39F2CC2C57C8
        switch (gameVersion) {
            case 0x4C07355966D815FB: std::cout << "Hello JPN00 (from plugin)" << std::endl; break;
            case 0x67C0F3042746D488: std::cout << "Hello JPN08 (from plugin)" << std::endl; break;
            case 0x49F643ADB6B18705: std::cout << "Hello JPN39 (from plugin)" << std::endl; break;
            case 0xA7EE39F2CC2C57C8: std::cout << "Hello CHN00 (from plugin)" << std::endl; break;
        }
        messageQueue.push ("Plugin got InitVersion!");
        mainLoopCv.notify_all ();
    }

    void InitCardReader (patches::Plugins::CommitCardCallback touch) {
        // New Callback API
        // Offers you a CardReader Callback function, save it and use at anywhere you wanna insert a card
        // callback available should be syncounized by UpdateStatus (type=1)
        commitCard = touch;
        // doc  : commitCard (std::string cardNumber, std::string chipId);
        // usage: commitCard ("00000000000000000001", "00000000000000000000000000000001");
        // usage: commitCard ("00000000000000000001", "");
        // usage: commitCard ("123", "");
        messageQueue.push ("Plugin got InitCardReader!");
        mainLoopCv.notify_all ();
    }

    void InitQRScanner (patches::Plugins::CommitQrCallback scan) {
        // New Callback API
        // Offers you a QrScanner Callback function, save it and use at anywhere you wanna insert qr data
        // callback available should be syncounized by UpdateStatus (type=2)
        commitQr = scan;
        // doc  : commitQr (std::vector<uint8_t> &qr_data);
        // usage: std::vector<uint8_t> data = {};   // insert bytes into this vector
        // usage: commitQr (data);                  // call commitQr send data
        messageQueue.push ("Plugin got InitQRScanner!");
        mainLoopCv.notify_all ();
    }

    void InitQRLogin (patches::Plugins::CommitQrLoginCallback login) {
        // New Callback API
        // Offers you a QrScanner Callback function, save it and use at anywhere you wanna insert card by qr (CHN)
        // callback available should be syncounized by UpdateStatus (type=2)
        commitQrLogin = login;
        // doc  : commitQrLogin (std::string cardNumber);
        // usage: commitQrLogin ("00000000000000000001");
        // usage: commitQrLogin ("123");
        messageQueue.push ("Plugin got InitQRLogin!");
        mainLoopCv.notify_all ();
    }

    void UpdateStatus (size_t type, bool status) {
        // New Callback API helper
        // Receive status change every time when game changes it's status
        // just believe it's control, if you find any issues, please contact TAL developers
        // Refer to https://github.com/esuo1198/TaikoArcadeLoader/blob/Refactor/src/constants.h
        switch (type) {
            case 1: cardStatus = status; break;
            case 2: qrStatus = status; break;
        }
        messageQueue.push (std::format ("Plugin got UpdateStatus type={} status={}!", type, status ? "true" : "false"));
        mainLoopCv.notify_all ();
    }

    // Lowlevel Card API
    void WaitTouch (patches::Plugins::CallBackTouchCard callback, uint64_t touchData) {
        // Called at Game got callback touch event, better use InitCardReader & InitQRLogin if you want to implement a card reader plugin
        // This is a low-level api which might cause some issue if you didn't use it properly
        // The specific usage please refer to Card part of https://github.com/esuo1198/TaikoArcadeLoader/blob/Refactor/src/patches/scanner.cpp
    }
}