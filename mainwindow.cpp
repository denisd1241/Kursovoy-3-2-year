#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLabel>
#include <QGridLayout>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QInputDialog>
#include <QQueue> // Add this for tree layout
#include <QRandomGenerator>
#include <QtMath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gridLayoutWidget_2->setStyleSheet("background-color: #a8a8a8; border: 1px solid #333;");
    ui->gridLayoutWidget->setStyleSheet("background-color: #a8a8a8; border: 1px solid #333;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

QVector<QVector<int>> createSymmetricMatrix(int size) {
    srand(time(0));

    QVector<QVector<int>> matrix(size, QVector<int>(size, 0));

    for (int i = 0; i < size; ++i) {
        for (int j = i; j < size; ++j) {
            int randomValue = rand() % 2;
            matrix[i][j] = randomValue;
            matrix[j][i] = randomValue;
        }
    }

    return matrix;
}

void MainWindow::on_pushButton_clicked(){
    QGridLayout *gridLayout = ui->gridLayout;

    bool ok;
    int matrixSize = QInputDialog::getInt(this, tr("Введите размер матрицы"), tr("Размер матрицы:"), 3, 1, 100, 1, &ok);

    if (ok) {
        clearGridLayout(gridLayout);
        QVector<QVector<int>> symmetricMatrix = createSymmetricMatrix(matrixSize);

        for (int i = 0; i < matrixSize; ++i) {
            for (int j = 0; j < matrixSize; ++j) {
                QLabel *label = new QLabel(QString::number(symmetricMatrix[i][j]));
                label->setStyleSheet("border: none;");
                label->setAlignment(Qt::AlignCenter);
                label->setMinimumSize(30, 30);
                label->setMaximumSize(30, 30);
                gridLayout->addWidget(label, i, j);
            }
        }

        // Рисуем граф
        drawGraph(symmetricMatrix);
    }
}

void MainWindow::clearGridLayout(QGridLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

void MainWindow::drawGraph(const QVector<QVector<int>>& matrix)
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);

    int nodeRadius = 20;

    QVector<QPoint> nodePositions(matrix.size());

    // Randomly place nodes
    for (int i = 0; i < matrix.size(); ++i) {
        int x = QRandomGenerator::global()->bounded(400); // Adjust the range as needed
        int y = QRandomGenerator::global()->bounded(400); // Adjust the range as needed

        nodePositions[i] = QPoint(x, y);

        QGraphicsEllipseItem *node = new QGraphicsEllipseItem(x, y, nodeRadius, nodeRadius);
        scene->addItem(node);

        // Add labels with vertex numbers
        QGraphicsTextItem *label = new QGraphicsTextItem(QString::number(i + 1));
        label->setPos(x + nodeRadius / 2 - label->boundingRect().width() / 2, y + nodeRadius / 2 - label->boundingRect().height() / 2);
        scene->addItem(label);
    }

    // Draw weighted edges based on the adjacency matrix
    for (int i = 0; i < matrix.size(); ++i) {
        for (int j = i; j < matrix.size(); ++j) {
            if (i != j && matrix[i][j] > 0) {  // Exclude self-loops
                int weight = QRandomGenerator::global()->bounded(1, 31); // Random weight between 1 and 30

                // Draw a path between nodes
                QPainterPath path;
                path.moveTo(nodePositions[i].x() + nodeRadius / 2, nodePositions[i].y() + nodeRadius / 2);
                path.lineTo(nodePositions[j].x() + nodeRadius / 2, nodePositions[j].y() + nodeRadius / 2);

                // Add the path to the scene with a default color
                scene->addPath(path);

                // Calculate position for the weight label
                int labelX = (nodePositions[i].x() + nodePositions[j].x()) / 2;
                int labelY = (nodePositions[i].y() + nodePositions[j].y()) / 2;

                // Add label with the weight to the scene
                QGraphicsTextItem *weightLabel = new QGraphicsTextItem(QString::number(weight));
                weightLabel->setPos(labelX - weightLabel->boundingRect().width() / 2, labelY - weightLabel->boundingRect().height() / 2);
                scene->addItem(weightLabel);

                // Optionally, set the text color to red
                weightLabel->setDefaultTextColor(Qt::red);
            }
        }
    }
}
