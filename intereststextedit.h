#ifndef INTERESTSTEXTEDIT_H
#define INTERESTSTEXTEDIT_H

#include <QPlainTextEdit>

class InterestsTextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit InterestsTextEdit(QWidget *parent = 0);

signals:

public slots:

protected:
    void keyPressEvent(QKeyEvent *event);
};

#endif // INTERESTSTEXTEDIT_H
