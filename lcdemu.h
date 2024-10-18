#ifndef LCDEMU_H
#define LCDEMU_H

#include <QWidget>
#include <QPainter>
#include <QKeyEvent>
#include <QTimer>
#include "swi2c.h"
#include "swspi.h"
#include "lcd_fonts.h"
#include "ssd1306.h"
#include "IP2365.h"
#include "sw35xx_s.h"
#include "INA3221.h"

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
    swi2c_t     *m_pSWi2c[4];
    swspi_t     *m_pSWspi1;
    SSD1306_t   *m_pSSD1306;
    ina3221_t   *m_pINA3221;
    IP2365_t    *m_pIP2365[2];
    sw35xx_t    *m_pSW3518[4];
    lcddev_t    *m_pLCD;

public slots:
    void        TimeEvent(void);

protected:
    void        paintEvent(QPaintEvent *event) override;
    void        keyPressEvent(QKeyEvent *event) override;
    //bool event(QEvent *event) override;

private:
    QTimer      m_qTimer;
    //SSD1306_t   *lcddev;
    QPixmap     m_pixmap;
    QImage      *m_pQimage;
    uint32_t    m_scale_w;
    uint32_t    m_scale_h;
    SSD1306_t   m_tSSD1306;
    swi2c_t     m_tSWi2c[4];
    swspi_t     m_tSWspi1;
    ina3221_t   m_tINA3221;
    IP2365_t    m_tIP2365[2];
    sw35xx_t    m_tSW3518[4];
    int         m_nWidx;
};

#endif // LCDEMU_H
