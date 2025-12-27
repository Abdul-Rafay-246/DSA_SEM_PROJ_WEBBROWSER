#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>

class SimpleBrowser : public QMainWindow {
public:
    SimpleBrowser() {
        QWidget *central = new QWidget;
        QVBoxLayout *mainLayout = new QVBoxLayout;

        tabs = new QTabWidget;
        mainLayout->addWidget(tabs);

        QHBoxLayout *bar = new QHBoxLayout;
        urlEdit = new QLineEdit;
        QPushButton *goBtn = new QPushButton("Go");
        QPushButton *addBtn = new QPushButton("+ Tab");
        QPushButton *closeBtn = new QPushButton("Close Tab");

        bar->addWidget(urlEdit);
        bar->addWidget(goBtn);
        bar->addWidget(addBtn);
        bar->addWidget(closeBtn);

        mainLayout->addLayout(bar);
        central->setLayout(mainLayout);
        setCentralWidget(central);

        connect(addBtn, &QPushButton::clicked, this, &SimpleBrowser::addTab);
        connect(closeBtn, &QPushButton::clicked, this, &SimpleBrowser::closeTab);
        connect(goBtn, &QPushButton::clicked, this, &SimpleBrowser::loadPage);

        addTab();
        resize(900, 700);
        setWindowTitle("Simple Browser (Qt)");
    }

private:
    QTabWidget *tabs;
    QLineEdit *urlEdit;

    void addTab() {
        QTextBrowser *view = new QTextBrowser;
        tabs->addTab(view, "Tab");
    }

    void closeTab() {
        int i = tabs->currentIndex();
        if (i >= 0)
            tabs->removeTab(i);
    }

    void loadPage() {
        QTextBrowser *view =
            qobject_cast<QTextBrowser *>(tabs->currentWidget());
        if (!view) return;

        QUrl url = QUrl::fromUserInput(urlEdit->text());
        view->setSource(url);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    SimpleBrowser browser;
    browser.show();
    return app.exec();
}
