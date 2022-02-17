#ifndef LOADER_H
#define LOADER_H

#include <QThread>

#include "mesh.h"

class Loader : public QThread
{
    Q_OBJECT
public:
    explicit Loader(QObject* parent, const QString& filename, bool is_reload);
    void run();

protected:
    Mesh* load_sml();

    Mesh* read_sml1(QFile& file);

signals:
    void loaded_file(QString filename);
    void got_mesh(Mesh* m, bool is_reload);

    void error_bad_stl();
    void error_empty_mesh();
    void error_missing_file();

private:
    const QString filename;
    bool is_reload;
};

#endif // LOADER_H
