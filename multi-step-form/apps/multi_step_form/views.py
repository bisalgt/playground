from django.shortcuts import render, redirect
from django.contrib import messages



from apps.multi_step_form import forms
from apps.multi_step_form.models import Book

def form_finder(request):
    if 'title' not in request.session.keys():
        form = forms.BookTitleForm()
    elif 'description' not in request.session.keys():
        form  = forms.BookDescriptionForm()
    elif 'category' not in request.session.keys():
        form = forms.BookCategoryForm()
    elif 'sub_category' not in request.session.keys():
        form  = forms.BookSubCategoryForm()            
    return form


def home(request):

    if request.method=='GET':
        # if 'title' not in request.session.keys():
        form = forms.BookTitleForm()
        books = Book.objects.all()
        return render(request, 'home.html', {'form': form, 'books': books})
        

    elif request.method=='POST':
        if 'title' in request.POST:
            form = forms.BookTitleForm(data=request.POST or None)
            if form.is_valid():
                title = form.cleaned_data.get('title')
                request.session['title'] = title
                form_next = form_finder(request)
                return render(request, 'home.html', {'form': form_next})
            return render(request, 'home.html', {'form': form})
        elif 'description' in request.POST:
            form  = forms.BookDescriptionForm(data=request.POST or None)
            if form.is_valid():
                description = form.cleaned_data.get('description')
                request.session['description'] = description
                form_next = form_finder(request)
                return render(request, 'home.html', {'form': form_next})
            return render(request, 'home.html', {'form': form})
        elif 'category' in request.POST:
            form = forms.BookCategoryForm(data=request.POST or None)
            if form.is_valid():
                category = form.cleaned_data.get('category')
                request.session['category'] = category
                form_next = form_finder(request)
                return render(request, 'home.html', {'form': form_next})
            return render(request, 'home.html', {'form': form})
        elif 'sub_category' in request.POST:
            print('inside sub categoryyy')
            form  = forms.BookSubCategoryForm(data=request.POST or None)
            if 'title' not in request.session:
                print('indise not title')
                return render(request, 'home.html', {'form': forms.BookTitleForm()})
            if form.is_valid():
                print('inside form valied')
                sub_category = form.cleaned_data.get('sub_category')
                title=request.session.pop('title')
                description=request.session.pop('description')
                category=request.session.pop('category')
                Book.objects.create(title=title, description=description, category=category, sub_category=sub_category)
                # print(Book.objects.all())
                # return render(request, 'success.html')
                messages.add_message(request, messages.SUCCESS, 'Successfully created new book.')
                return redirect('/')
            print('form not valid last line')
            return render(request, 'home.html', {'form': form})

        
