def conv2d_greyscale_img(img_array, _filter, padding, stride):

    img_row_len, img_col_len = img_array.shape
    fltr_len = _filter.shape[0] # filter is a square matrix as row len == col len
    
    output_row_size = ((img_row_len - fltr_len + 2*padding)/stride) + 1  # if decimal then invalid padding / filter / stride
    output_col_size = ((img_col_len - fltr_len + 2*padding)/stride) + 1
    
    if (output_row_size != int(output_row_size)) or (output_col_size != int(output_col_size)):
        raise Exception("row and col size should be an integers not float")
        
    output_row_size = int(output_row_size)
    output_col_size = int(output_col_size)

    result = np.zeros((output_row_size, output_col_size))

    for i in tqdm(range(img_row_len - fltr_len + 1)):
        for j in range(img_col_len - fltr_len + 1):
            result[i, j] = np.sum(np.multiply(img_array[i:i+fltr_len, j:j+fltr_len], _filter))
    return result


