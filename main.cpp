#include <QApplication>
#include "mediaplayer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    MediaPlayer player;
    player.show();
    return app.exec();
}
