# defect detection middle block
Middle block, which uses pytorch model.
Модуль, который берёт заранее обученную нейросеть для поиска дефектов и применяет её для данных?

# Attention!
Этот блок в разработке. Пока всё, что он делает -- берет все изображения из шареной памяти и выводит в лог то, сколько единиц в "маске", которую выдала нейронка.

Что бы хотелось, но не реализованно:

1. "Договориться" какие именно картинки, попавшие в шареную память, обрабатывать.
2. Нужно ли, чтобы этот блок что-то сохранял куда-то? Пока он выводит в ЛОГ часть данных.
  * Но другому блоку может понадобиться сама маска, что означает сохранение в шареную память. Внутрь того же frameset? Как модифицировать название картинки? Оставлять ли timestamp как у исходной картинки или же менять на текущее время? На эти вопросы нужно ответить, когда станет ясно, что делает следующий блок
  * Можно сохранять ЛОГ. Или ещё что-то
3. Нужно ли как-то сигнализировать, о том, что маска не пуста? Ведь, если маска не пуста, это означает, что нейронка нашла на картинке дефект.