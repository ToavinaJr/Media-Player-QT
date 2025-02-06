#ifndef MEDIAPLAYER_H
#define MEDIAPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoWidget>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QStyle>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QMessageBox>
#include <QStandardPaths>

class MediaPlayer : public QMainWindow {
    Q_OBJECT

public:
    explicit MediaPlayer(QWidget *parent = nullptr);
    ~MediaPlayer();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    // Composants multimédias
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    QVideoWidget *videoWidget;

    // Interface utilisateur
    QWidget *titleBar;
    QWidget *controlArea;
    QSlider *timeSlider;
    QSlider *volumeSlider;
    QPushButton *openButton;
    QPushButton *playButton;
    QPushButton *stopButton;
    QLabel *currentTimeLabel;
    QLabel *totalTimeLabel;
    bool sliderIsPressed;

    // Méthodes privées
    void createTitleBar();
    void createControlArea();
    void connectSignalsSlots();
    void loadStyle();
    QString formatTime(qint64 milliseconds);
    QPushButton* createButton(QStyle::StandardPixmap icon);

private slots:
    // Gestion des contrôles
    void openFile();
    void togglePlayPause();
    void toggleMaximized();
    void setVolume(int volume);

    // Gestion du média
    void updateDuration(qint64 duration);
    void updatePosition(qint64 position);
    void updatePlaybackState(QMediaPlayer::PlaybackState state);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);

    // Gestion du slider
    void handleSliderValueChanged(int value);
    void handleSliderPressed();
    void handleSliderReleased();
};

#endif // MEDIAPLAYER_H
