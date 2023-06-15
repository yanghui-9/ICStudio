#ifndef LABELBASE_H
#define LABELBASE_H

#include <QGroupBox>

class QTextEdit;
class LabelBase : public QGroupBox
{
    Q_OBJECT
public:
    explicit LabelBase(QWidget *parent = nullptr);

    QTextEdit * textEdit;
};

#endif // LABELBASE_H
