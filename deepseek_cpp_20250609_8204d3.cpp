#include <iostream>
#include <cmath>
#include <map>

using namespace std;

const int imageLengthSegmentation = 40;
const int imageWidthSegmentation = 40;

int videoCameraImageSegmentation[imageWidthSegmentation][imageLengthSegmentation] = {
    // ... (ваш исходный массив данных)
};

int main() {
    setlocale(LC_ALL, "ru");

    int processedImage[imageWidthSegmentation][imageLengthSegmentation];
    bool visited[imageWidthSegmentation][imageLengthSegmentation] = { {false} };

    // Копируем исходное изображение
    for (int j = 0; j < imageWidthSegmentation; j++) {
        for (int i = 0; i < imageLengthSegmentation; i++) {
            processedImage[j][i] = videoCameraImageSegmentation[j][i];
        }
    }

    const int QUEUE_SIZE = 1600;
    int queueY[QUEUE_SIZE];
    int queueX[QUEUE_SIZE];
    const int COLOR_DIFF_THRESHOLD = 2;

    // Обработка плавных переходов
    for (int j = 0; j < imageWidthSegmentation; j++) {
        for (int i = 0; i < imageLengthSegmentation; i++) {
            if (visited[j][i] || processedImage[j][i] == 115)
                continue;

            int front = 0, rear = 0;
            map<int, int> valueCounts;

            queueY[rear] = j;
            queueX[rear] = i;
            rear++;
            visited[j][i] = true;
            valueCounts[processedImage[j][i]]++;

            while (front < rear) {
                int y = queueY[front];
                int x = queueX[front];
                front++;

                int dx[] = { 0, 0, 1, -1 };
                int dy[] = { 1, -1, 0, 0 };

                for (int d = 0; d < 4; d++) {
                    int ny = y + dy[d];
                    int nx = x + dx[d];

                    if (ny < 0 || ny >= imageWidthSegmentation ||
                        nx < 0 || nx >= imageLengthSegmentation)
                        continue;

                    // Пропускаем только пиксели со значением 115
                    if (processedImage[ny][nx] == 115)
                        continue;

                    if (visited[ny][nx])
                        continue;

                    // Проверяем, что соседний пиксель либо того же значения, либо отличается не более чем на порог
                    // И при этом он не граничит с другой областью (кроме текущей)
                    int currentValue = processedImage[y][x];
                    int neighborValue = processedImage[ny][nx];
                    
                    if (abs(currentValue - neighborValue) <= COLOR_DIFF_THRESHOLD) {
                        // Проверяем соседей этого пикселя на наличие других областей
                        bool hasOtherRegion = false;
                        for (int dd = 0; dd < 4; dd++) {
                            int nny = ny + dy[dd];
                            int nnx = nx + dx[dd];
                            
                            if (nny < 0 || nny >= imageWidthSegmentation ||
                                nnx < 0 || nnx >= imageLengthSegmentation)
                                continue;
                                
                            if (processedImage[nny][nnx] != 115 && 
                                processedImage[nny][nnx] != currentValue && 
                                abs(processedImage[nny][nnx] - currentValue) > COLOR_DIFF_THRESHOLD) {
                                hasOtherRegion = true;
                                break;
                            }
                        }
                        
                        if (!hasOtherRegion) {
                            visited[ny][nx] = true;
                            queueY[rear] = ny;
                            queueX[rear] = nx;
                            rear++;
                            valueCounts[neighborValue]++;
                        }
                    }
                }
            }

            // Находим доминирующее значение
            int dominantValue = processedImage[j][i];
            int maxCount = 0;
            for (const auto& pair : valueCounts) {
                if (pair.second > maxCount) {
                    maxCount = pair.second;
                    dominantValue = pair.first;
                }
            }

            // Заменяем все пиксели области на доминирующее значение
            for (int idx = 0; idx < rear; idx++) {
                processedImage[queueY[idx]][queueX[idx]] = dominantValue;
            }
        }
    }

    // Вторая проверка: если пиксель не граничит ни с одной областью (все соседи 115), оставляем его значение
    for (int j = 0; j < imageWidthSegmentation; j++) {
        for (int i = 0; i < imageLengthSegmentation; i++) {
            if (processedImage[j][i] == 115) continue;
            
            bool allNeighbors115 = true;
            int dx[] = { 0, 0, 1, -1 };
            int dy[] = { 1, -1, 0, 0 };
            
            for (int d = 0; d < 4; d++) {
                int ny = j + dy[d];
                int nx = i + dx[d];
                
                if (ny < 0 || ny >= imageWidthSegmentation ||
                    nx < 0 || nx >= imageLengthSegmentation)
                    continue;
                    
                if (processedImage[ny][nx] != 115) {
                    allNeighbors115 = false;
                    break;
                }
            }
            
            if (allNeighbors115) {
                processedImage[j][i] = videoCameraImageSegmentation[j][i];
            }
        }
    }

    // Вывод обработанного изображения
    cout << "Обработанное изображение:" << endl;
    for (int j = 0; j < imageWidthSegmentation; j++) {
        for (int i = 0; i < imageLengthSegmentation; i++) {
            int value = processedImage[j][i];

            if (value >= 0 && value < 10)
                cout << "  " << value << ' ';
            else if (value >= 10 && value < 100)
                cout << " " << value << ' ';
            else
                cout << value << ' ';
        }
        cout << endl;
    }

    return 0;
}