#include "mediaplayer.h"
#include <QStandardPaths>
#include <QMessageBox>
#include <QMouseEvent>
#include <QStyle>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTime>

MediaPlayer::MediaPlayer(QWidget *parent)
    : QMainWindow(parent), sliderIsPressed(false) {

    // Configuration des composants multimédias
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    videoWidget = new QVideoWidget(this);

    // Configuration cruciale pour le rendu vidéo
    videoWidget->setAttribute(Qt::WA_NativeWindow);
    videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    player->setAudioOutput(audioOutput);
    player->setVideoOutput(videoWidget);

    // Initialisation de l'interface
    createControlArea();

    // Layout principal
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(videoWidget, 1);
    mainLayout->addWidget(controlArea, 0);
    mainLayout->setContentsMargins(2, 2, 2, 2);

    setCentralWidget(centralWidget);
    resize(800, 600);

    // Configuration initiale
    audioOutput->setVolume(0.5); // Volume à 50%
    player->setLoops(QMediaPlayer::Once); // Lecture unique par défaut

    // Connexions et style
    connectSignalsSlots();
    loadStyle();
}

MediaPlayer::~MediaPlayer() {
    player->stop();
}

void MediaPlayer::createControlArea() {
    controlArea = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(controlArea);

    // Boutons
    openButton = createButton(QStyle::SP_DirOpenIcon);
    playButton = createButton(QStyle::SP_MediaPlay);
    stopButton = createButton(QStyle::SP_MediaStop);

    // Sliders
    timeSlider = new QSlider(Qt::Horizontal);
    timeSlider->setRange(0, 0);
    timeSlider->setTracking(true);
    timeSlider->setPageStep(5000); // Pas de 5 secondes
    timeSlider->setSingleStep(1000); // Pas de 1 seconde
    timeSlider->installEventFilter(this);

    volumeSlider = new QSlider(Qt::Horizontal);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(50);

    // Labels temporels
    currentTimeLabel = new QLabel("00:00");
    totalTimeLabel = new QLabel("00:00");

    // Organisation
    layout->addWidget(openButton);
    layout->addWidget(playButton);
    layout->addWidget(stopButton);
    layout->addWidget(currentTimeLabel);
    layout->addWidget(timeSlider, 1);
    layout->addWidget(volumeSlider);
    layout->addWidget(totalTimeLabel);
}

void MediaPlayer::connectSignalsSlots() {
    // Contrôles
    connect(openButton, &QPushButton::clicked, this, &MediaPlayer::openFile);
    connect(playButton, &QPushButton::clicked, this, &MediaPlayer::togglePlayPause);
    connect(stopButton, &QPushButton::clicked, player, &QMediaPlayer::stop);
    connect(volumeSlider, &QSlider::valueChanged, this, &MediaPlayer::setVolume);

    // Slider temporel
    connect(timeSlider, &QSlider::sliderPressed, this, &MediaPlayer::handleSliderPressed);
    connect(timeSlider, &QSlider::sliderReleased, this, &MediaPlayer::handleSliderReleased);
    connect(timeSlider, &QSlider::valueChanged, this, &MediaPlayer::handleSliderValueChanged);

    // Signaux multimédias
    connect(player, &QMediaPlayer::durationChanged, this, &MediaPlayer::updateDuration);
    connect(player, &QMediaPlayer::positionChanged, this, &MediaPlayer::updatePosition);
    connect(player, &QMediaPlayer::playbackStateChanged, this, &MediaPlayer::updatePlaybackState);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MediaPlayer::onMediaStatusChanged);
    connect(player, &QMediaPlayer::errorOccurred, this, [this](QMediaPlayer::Error error, const QString &errorString) {
        QMessageBox::critical(this, "Erreur multimédia",
                              QString("Code erreur: %1\n%2").arg(error).arg(errorString));
    });
}

void MediaPlayer::loadStyle() {
    setStyleSheet(R"(
        QMainWindow {
            background: #2d2d2d;
            border: 1px solid #404040;
            border-radius: 6px;
        }
        #titleBar {
            background: #353535;
            border-top-left-radius: 5px;
            border-top-right-radius: 5px;
        }
        QSlider::handle:horizontal {
            background: #1abc9c;
            width: 12px;
            margin: -4px 0;
            border-radius: 6px;
        }
        QSlider::groove:horizontal {
            background: #404040;
            height: 4px;
            border-radius: 2px;
        }
        QLabel {
            color: #cccccc;
            padding: 0 5px;
        }
    )");
}

bool MediaPlayer::eventFilter(QObject *obj, QEvent *event) {
    if (obj == timeSlider && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        int newPosition = QStyle::sliderValueFromPosition(
            timeSlider->minimum(),
            timeSlider->maximum(),
            mouseEvent->position().toPoint().x(),
            timeSlider->width()
            );

        timeSlider->setValue(newPosition);
        player->setPosition(newPosition);

        return true;
    }

    // Gestion du clic sur le slider de volume
    if (obj == volumeSlider && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

        int newVolume = QStyle::sliderValueFromPosition(
            volumeSlider->minimum(),
            volumeSlider->maximum(),
            mouseEvent->position().toPoint().x(),
            volumeSlider->width()
            );

        volumeSlider->setValue(newVolume);
        setVolume(newVolume);

        return true;
    }
    return QMainWindow::eventFilter(obj, event);
}

void MediaPlayer::openFile() {
    QString path = QFileDialog::getOpenFileName(this,
                                                "Ouvrir un fichier multimédia",
                                                QStandardPaths::writableLocation(QStandardPaths::MoviesLocation),
                                                "Fichiers multimédias (*.mp4 *.avi *.mkv *.mp3 *.wav)");

    if (!path.isEmpty()) {
        player->setSource(QUrl::fromLocalFile(path));
        player->play();

        timeSlider->setValue(0);
        currentTimeLabel->setText("00:00");
        totalTimeLabel->setText("00:00");
    }
}

QString MediaPlayer::formatTime(qint64 milliseconds) {
    qint64 seconds = milliseconds / 1000;
    qint64 minutes = seconds / 60;
    seconds %= 60;
    return QString("%1:%2")
        .arg(minutes, 2, 10, QLatin1Char('0'))
        .arg(seconds, 2, 10, QLatin1Char('0'));
}

QPushButton* MediaPlayer::createButton(QStyle::StandardPixmap icon) {
    QPushButton *button = new QPushButton(this);
    button->setIcon(style()->standardIcon(icon));
    button->setIconSize(QSize(32, 32));
    button->setStyleSheet(
        "QPushButton {"
        "   background: transparent;"
        "   border: 2px solid #1abc9c;"
        "   border-radius: 8px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background: rgba(26, 188, 156, 0.3);"
        "}");
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

void MediaPlayer::togglePlayPause() {
    if(player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    } else {
        player->play();
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    }
}

void MediaPlayer::setVolume(int volume) {
    // Limiter la valeur entre 0 et 100
    volume = qBound(0, volume, 100);

    // Mettre à jour le volume
    audioOutput->setVolume(volume / 100.0f);

    // Optionnel : feedback visuel
    volumeSlider->blockSignals(true);
    volumeSlider->setValue(volume);
    volumeSlider->blockSignals(false);
}

void MediaPlayer::updateDuration(qint64 duration) {
    timeSlider->setMaximum(duration);
    totalTimeLabel->setText(formatTime(duration));
}

void MediaPlayer::updatePosition(qint64 position) {
    if (!sliderIsPressed) {
        timeSlider->blockSignals(true);
        timeSlider->setValue(position);
        timeSlider->blockSignals(false);
        currentTimeLabel->setText(formatTime(position));
    }
}

void MediaPlayer::updatePlaybackState(QMediaPlayer::PlaybackState state) {
    switch(state) {
    case QMediaPlayer::PlayingState:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        break;
    case QMediaPlayer::PausedState:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        break;
    case QMediaPlayer::StoppedState:
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        timeSlider->setValue(0);
        currentTimeLabel->setText("00:00");
        break;
    }
}

void MediaPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        timeSlider->setValue(0);
        currentTimeLabel->setText("00:00");
    }
}

void MediaPlayer::toggleMaximized() {
    isMaximized() ? showNormal() : showMaximized();
}

void MediaPlayer::handleSliderPressed() {
    sliderIsPressed = true;
    player->pause();
}

void MediaPlayer::handleSliderReleased() {
    sliderIsPressed = false;
    player->setPosition(timeSlider->value());
    if (player->playbackState() == QMediaPlayer::PausedState) {
        player->play();
    }
}

void MediaPlayer::handleSliderValueChanged(int value) {
    if (sliderIsPressed) {
        player->setPosition(value);
        currentTimeLabel->setText(formatTime(value));
    }
}
