from django.db import models


class Book(models.Model):
    title = models.CharField(max_length=255)
    description = models.TextField()
    category = models.CharField(max_length=20)
    sub_category = models.CharField(max_length=20)
    
    def __str__(self):
        return self.title

