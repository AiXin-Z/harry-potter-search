#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include"bmsearch.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QString readBookFile(const QString &bookFilePath);
    // mainwindow.cpp
    void parseBookStructure(const QString& bookName, const QString& content);
    int getActualPage(const QString& bookName, int pos);
    QString getActualChapter(const QString& bookName, int pos);
    void showContextDialog(const QString& title, const QString& htmlContent);

private slots:
    void on_quaryButton_clicked();
    void onResultTableClicked(int row, int column);

private:
    struct BookStructure
    {
        //位置 章节名
        QMap<int, QString> chapters;
        //位置 页码
        QMap<int, int> pages;
        //页码分隔位置
        QVector<int> pageBreaks;
        //章节分隔位置
        QVector<int> chapterBreaks;
    };

    QMap<QString, BookStructure> bookStructures;
    Ui::MainWindow *ui;
    BMsearch bmSearchInstance;
    QStringList bookFiles;
    QMap<QString, QString> bookContentsMap;

};
#endif // MAINWINDOW_H
