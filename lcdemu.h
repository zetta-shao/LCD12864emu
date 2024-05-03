#ifndef LCDEMU_H
#define LCDEMU_H

#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include "ssd1306.h"

using namespace std;

class LCDemu : public QWidget {
    Q_OBJECT

public:
    explicit    LCDemu(QWidget *parent);
    ~LCDemu(void);
    void        update();
    void        setlcd(SSD1306_t *p);
    void        start(void);
    void        task_key1(void);
    void        task_key2(void);
    void        task_key3(void);
    void        task_key4(void);

protected:
    void        paintEvent(QPaintEvent *event) override;
    void        keyPressEvent(QKeyEvent *event) override;
    //bool event(QEvent *event) override;

private:
    SSD1306_t   *lcddev;
    QPixmap     m_pixmap;
    QImage      *m_pQimage;
    uint32_t    m_scale_w;
    uint32_t    m_scale_h;
};

#endif // LCDEMU_H
