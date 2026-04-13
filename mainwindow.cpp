#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QString>
#include <QDebug>
#include <QTableWidgetItem>
#include <QFileInfo>
#include<QVBoxLayout>
#include<QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->resultsTableWidget->setColumnCount(6);
    QStringList headers = {"序号", "人名/地名", "页码", "章节", "书名", "位置"};
    ui->resultsTableWidget->setHorizontalHeaderLabels(headers);
    ui->resultsTableWidget->setColumnHidden(5, true);//隐藏位置列
    //连接槽函数
    connect(ui->resultsTableWidget, &QTableWidget::cellClicked, this, &MainWindow::onResultTableClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//读取文本
QString MainWindow::readBookFile(const QString &bookFilePath)
{
    QFile file(bookFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }
    QTextStream in(&file);
    QString content = in.readAll();
    file.close();
    return content;
}

//获取页码信息
int MainWindow::getActualPage(const QString& bookName, int pos)
{
    auto& pages = bookStructures[bookName].pages;
    auto it = pages.upperBound(pos);
    return (it == pages.begin()) ? 1 : (--it).value();
}

//获取章节信息
QString MainWindow::getActualChapter(const QString& bookName, int pos)
{
    auto& chapters = bookStructures[bookName].chapters;
    auto it = chapters.upperBound(pos);
    return (it == chapters.begin()) ? "Prologue" : (--it).value();
}

//当点击查询按钮
void MainWindow::on_quaryButton_clicked()
{
    //清空旧数据
    ui->resultsTableWidget->clearContents();
    ui->resultsTableWidget->setRowCount(0);
    bookContentsMap.clear();
    bookStructures.clear();

    //设置表格
    // 显示查找到的人名/地名，以及出现的页码和章节，书名， 按照出现的页码顺序显示，每个查询结果都对应序号。

    //             序号       人名/地名          页码        章节        书名

    //              1          Harry            1           1      Harry_Potter_and_the_Chamber_of_Secrets_Book_2
    ui->resultsTableWidget->setColumnCount(6);
    QStringList headers = {"序号", "关键词", "页码", "章节", "书名", "位置"};
    ui->resultsTableWidget->setHorizontalHeaderLabels(headers);
    ui->resultsTableWidget->setColumnHidden(5, true);
    ui->resultsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    //获取关键词
    QString keyword = ui->keywordEdit->text().trimmed();
    if (keyword.isEmpty())
    {
        QMessageBox::warning(this, "错误", "请输入查询关键词");
        return;
    }

    //书籍文件列表，要用相对路径
    //这里我改动了一下书籍的名字，因为有点或者是别的符号qt好像识别不了？
    QStringList bookFiles =
    {
        "../../HP0_Harry_Potter_Prequel.txt",
        "../../HP2--Harry_Potter_and_the_Chamber_of_Secrets_Book_2_.txt",
        "../../HP3_Harry_Potter_and_the_Prisoner_of_Azkaban.txt",
        "../../HP4_Harry_Potter_and_the_Goblet_of_Fire.txt",
        "../../HP6_Harry_Potter_and_the_Half_Blood_Prince.txt",
        "../../HP7--Harry_Potter_and_the_Deathly_Hallows_Book_7_.txt",
        "../../Quidditch_Through_the_Ages.txt",
        "../../The_Tales_of_Beedle_the_Bard.txt"
    };
    //结果序号
    int resultIndex = 1;

    //遍历所有书籍
    for (const QString& bookFile : bookFiles)
    {
        // 读取内容
        QString content = readBookFile(bookFile);
        if (content.isEmpty()) continue;

        QString bookName = QFileInfo(bookFile).baseName();
        bookContentsMap[bookName] = content;

        //解析书籍章节，页码
        parseBookStructure(bookName, content);

        //进行BM搜索
        BMsearch bm(keyword.toStdString());
        std::vector<int> matches = bm.search(content.toStdString());

        //处理每个匹配项
        for (int bytePos : matches)
        {
            //转换为UTF-16字符位置，要不然总是匹配不上
            QByteArray utf8Data = content.toUtf8().left(bytePos);
            int charPos = QString::fromUtf8(utf8Data).length();

            //验证位置有效性（修改之前会出现当前位置无效数据的报错）
            if (charPos >= content.length())
            {
                qDebug() << "无效位置：" << bytePos << "->" << charPos;
                continue;
            }

            //获取页码和章节
            int page = getActualPage(bookName, charPos);
            QString chapter = getActualChapter(bookName, charPos);

            //插入表格，来显示查询结果
            int row = ui->resultsTableWidget->rowCount();
            ui->resultsTableWidget->insertRow(row);

            //填充各列数据，ui显示
            ui->resultsTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(resultIndex)));
            ui->resultsTableWidget->setItem(row, 1, new QTableWidgetItem(keyword));
            ui->resultsTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(page)));
            ui->resultsTableWidget->setItem(row, 3, new QTableWidgetItem(chapter));
            ui->resultsTableWidget->setItem(row, 4, new QTableWidgetItem(bookName));
            ui->resultsTableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(charPos)));

            resultIndex++;
        }
    }

    if (resultIndex == 1)
    {
        QMessageBox::information(this, "无结果", "未找到相关内容");
    }
}
// #include <ctime>  // 包含头文件

// void MainWindow::on_quaryButton_clicked()
// {
//     // 获取查询开始时间
//     clock_t startTime = clock();

//     // 清空旧数据
//     ui->resultsTableWidget->clearContents();
//     ui->resultsTableWidget->setRowCount(0);
//     bookContentsMap.clear();
//     bookStructures.clear();

//     ui->resultsTableWidget->setColumnCount(6);
//     QStringList headers = {"序号", "关键词", "页码", "章节", "书名", "位置"};
//     ui->resultsTableWidget->setHorizontalHeaderLabels(headers);
//     ui->resultsTableWidget->setColumnHidden(5, true);
//     ui->resultsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

//     // 获取关键词
//     QString keyword = ui->keywordEdit->text().trimmed();
//     if (keyword.isEmpty()) {
//         QMessageBox::warning(this, "错误", "请输入查询关键词");
//         return;
//     }

//     // 书籍文件列表
//     QStringList bookFiles = {
//         "../../HP0_Harry_Potter_Prequel.txt",
//         "../../HP2--Harry_Potter_and_the_Chamber_of_Secrets_Book_2_.txt",
//         "../../HP3_Harry_Potter_and_the_Prisoner_of_Azkaban.txt",
//         "../../HP4_Harry_Potter_and_the_Goblet_of_Fire.txt",
//         "../../HP6_Harry_Potter_and_the_Half_Blood_Prince.txt",
//         "../../HP7--Harry_Potter_and_the_Deathly_Hallows_Book_7_.txt",
//         "../../Quidditch_Through_the_Ages.txt",
//         "../../The_Tales_of_Beedle_the_Bard.txt"
//     };

//     int resultIndex = 1; // 结果序号

//     // 遍历所有书籍
//     for (const QString& bookFile : bookFiles) {
//         // 读取内容
//         QString content = readBookFile(bookFile);
//         if (content.isEmpty()) continue;

//         QString bookName = QFileInfo(bookFile).baseName();
//         bookContentsMap[bookName] = content;

//         // 解析书籍结构（章节/页码）
//         parseBookStructure(bookName, content);

//         // BM搜索
//         BMsearch bm(keyword.toStdString());
//         std::vector<int> matches = bm.search(content.toStdString());

//         // 处理每个匹配项
//         for (int bytePos : matches) {
//             // 转换为UTF-16字符位置
//             QByteArray utf8Data = content.toUtf8().left(bytePos);
//             int charPos = QString::fromUtf8(utf8Data).length();

//             // 验证位置有效性（修改之前会出现当前位置无效数据的报错）
//             if (charPos >= content.length()) {
//                 qDebug() << "无效位置：" << bytePos << "->" << charPos;
//                 continue;
//             }

//             // 获取页码和章节
//             int page = getActualPage(bookName, charPos);
//             QString chapter = getActualChapter(bookName, charPos);

//             // 插入表格，来显示查询结果
//             int row = ui->resultsTableWidget->rowCount();
//             ui->resultsTableWidget->insertRow(row);

//             // 填充各列数据，ui显示
//             ui->resultsTableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(resultIndex)));
//             ui->resultsTableWidget->setItem(row, 1, new QTableWidgetItem(keyword));
//             ui->resultsTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(page)));
//             ui->resultsTableWidget->setItem(row, 3, new QTableWidgetItem(chapter));
//             ui->resultsTableWidget->setItem(row, 4, new QTableWidgetItem(bookName));
//             ui->resultsTableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(charPos)));

//             resultIndex++;
//         }
//     }

//     // 如果没有结果
//     if (resultIndex == 1) {
//         QMessageBox::information(this, "无结果", "未找到相关内容");
//     }

//     // 获取查询结束时间
//     clock_t endTime = clock();

//     // 计算查询所用时间，并输出
//     double duration = (double)(endTime - startTime) / CLOCKS_PER_SEC;
//     qDebug() << "times: " << duration << "s"; // 在调试窗口输出

//     // 自动调整列宽
//     ui->resultsTableWidget->resizeColumnsToContents();
// }

//选择查询结果记录项时，显示指定的人名/地名位置前后的一段文字
//选择序号，或者单击查询记录行，能够显示到指定位置人名/地名前后的一段文字
//当点击表格中任意行的数据，显示当前行的查找结果
void MainWindow::onResultTableClicked(int row, int)
{
    //边界的检查
    if (row < 0 || row >= ui->resultsTableWidget->rowCount()) return;

    //获取书籍相关的数据
    QTableWidgetItem* itemBook = ui->resultsTableWidget->item(row, 4);
    QTableWidgetItem* itemPos = ui->resultsTableWidget->item(row, 5);
    QTableWidgetItem* itemKeyword = ui->resultsTableWidget->item(row, 1);
    if (!itemBook || !itemPos || !itemKeyword) return;

    QString bookName = itemBook->text();
    QString keyword = itemKeyword->text();
    int charPos = itemPos->text().toInt();

    //获取书籍内容
    if (!bookContentsMap.contains(bookName)) return;
    QString content = bookContentsMap[bookName];

    //获取书籍的结构和名字
    if (!bookStructures.contains(bookName)) return;
    BookStructure& structure = bookStructures[bookName];

    //设置显示上下文的字符范围，显示关键词前后的一些文字
    const int contextLength = 200;

    //关键词所在位置的起始位置和结束位置
    //不能小于0也不能超过内容的长度
    int startPos = qMax(0, charPos - contextLength);
    int endPos = qMin(content.length(), charPos + keyword.length() + contextLength);

    //提取关键词所在位置的上下文内容
    QString context = content.mid(startPos, endPos - startPos);

    //设置查找的关键词在文本上下文中显示红色，便于找到，这里要用到html的相关知识
    context.replace(keyword, "<b><font color='red'>" + keyword + "</font></b>", Qt::CaseInsensitive);

    //显示内容
    QString contextDisplay = "<pre>" + context.toHtmlEscaped() + "</pre>";

    //显示弹窗
    QString title = "第 " + QString::number(getActualPage(bookName, charPos)) + " 页 - 上下文";
    showContextDialog(title, context);
}

//解析文本结构，将页数和章节信息存储，并显示在表格里
void MainWindow::parseBookStructure(const QString& bookName, const QString& content)
{
    //定义书籍的数据结构
    BookStructure structure;
    //保留空行以正确计算位置，按照换行来分
    QStringList lines = content.split('\n', Qt::KeepEmptyParts);
    //当前字符的位置为0
    int currentPos = 0;

    //阅读文本可以发现，有的章节是CHAPTER ONE格式，有的就是CHAPTER one，不太统一，所以要多种格式的章节
    QRegularExpression chapterRegex
        (
        R"(^(CHAPTER|Chapter|chapter)[\s_]+([IVXLCDM]+|\d+|ONE|TWO|THREE)\b)",
        QRegularExpression::CaseInsensitiveOption//忽略大小写
        );

    //页码的匹配是纯数字，并且书籍中的数字页码后面一般都跟着换行符的出现
    QRegularExpression pageRegex(R"(\b(\d+)\b)");

    for (int i = 0; i < lines.size(); ++i)
    {
        //去除首尾的空白
        QString line = lines[i].trimmed();

        //匹配章节
        QRegularExpressionMatch chapterMatch = chapterRegex.match(line);
        if (chapterMatch.hasMatch())
        {
            //保留原始的格式
            QString fullChapter = line;
            structure.chapters.insert(currentPos, fullChapter);
            structure.chapterBreaks.append(currentPos);
        }

        //匹配页码
        QRegularExpressionMatch pageMatch = pageRegex.match(line);
        if (pageMatch.hasMatch())
        {
            bool ok;
            int page = pageMatch.captured(1).toInt(&ok);
            if (ok)
            {
                structure.pages.insert(currentPos, page);
                structure.pageBreaks.append(currentPos);
            }
        }

        currentPos += line.length() + 1;//+1是换行符
    }

    bookStructures[bookName] = structure;
}

//显示函数
void MainWindow::showContextDialog(const QString& title, const QString& htmlContent)
{
    //html的模态对话框
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle(title);
    //设置一个合适的大小
    dialog->resize(700, 400);

    QVBoxLayout* layout = new QVBoxLayout(dialog);

    QTextEdit* textEdit = new QTextEdit(dialog);
    //设置为只读
    textEdit->setReadOnly(true);
    textEdit->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    textEdit->setHtml(htmlContent);
    layout->addWidget(textEdit);

    //关闭对话框
    QPushButton* closeBtn = new QPushButton("关闭", dialog);
    connect(closeBtn, &QPushButton::clicked, dialog, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignRight);

    dialog->setLayout(layout);
    //模态显示
    dialog->exec();
}
