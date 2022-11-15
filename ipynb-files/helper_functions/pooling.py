def calculate_output_size_after_pooling(input_size, pool_size):
    remainder, quotient = input_size % pool_size[0], input_size // pool_size[0]
    if quotient >= 1:
        if remainder > 0:
            output_size = quotient + 1
        else:
            output_size = quotient
    else:
        output_size = 1
    return output_size

def pooling(img_array, pool_size, mode="max"):
    row, col = img_array.shape
    output_row_size = calculate_output_size_after_pooling(row, pool_size)
    output_col_size = calculate_output_size_after_pooling(col, pool_size)
    print(output_row_size, output_col_size)
    result = np.zeros((output_row_size, output_col_size))

    for i in tqdm(range(output_row_size)):
        for j in range(output_col_size):
            start_row = pool_size[0]*i
            end_row = pool_size[0]*i+pool_size[0]
            start_col = pool_size[1]*j
            end_col = pool_size[1]*j+pool_size[1]
            # print(img_array[start_row:end_row, start_col:end_col])
            # print("-------", start_row, end_row, start_col, end_col)
            if mode == "max":
                result[i, j] = np.max(img_array[start_row:end_row, start_col:end_col])
            elif mode == "average":
                result[i, j] = np.mean(img_array[start_row:end_row, start_col:end_col])
        # print("+++++++++")

            # print(np.max(img_array[i:i+2, j:j+2]))
            
    return result
