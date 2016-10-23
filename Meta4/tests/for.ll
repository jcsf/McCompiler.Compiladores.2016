declare i32 @atoi(i8* nocapture) nounwind readonly
@.stritoa = private unnamed_addr constant [3 x i8] c"%d\00"
define i8* @itoa(i32 %n, i8* %buf) {
	%1 = alloca i32
	%2 = alloca i8*
	store i32 %n, i32* %1
	store i8* %buf, i8** %2
	%3 = load i8** %2
	%4 = load i32* %1
	%5 = call i32 (i8*, i8*, ...)* @sprintf(i8* %3, i8* getelementptr inbounds ([3 x i8]* @.stritoa, i32 0, i32 0), i32 %4)
	%6 = load i8** %2
	ret i8* %6
}
declare i32 @sprintf(i8*, i8*, ...)
declare i32 @puts(i8* nocapture) nounwind
define i32 @main (i32 %argc, i8** %argv) {
	%1 = alloca i32
	store i32 %argc, i32* %1
	%2 = alloca i8**
	store i8** %argv, i8*** %2
	%x = alloca i32
	%y = alloca i32
	%i = alloca i32
	%j = alloca i32
	%buf = alloca [20 x i8]
	%3 = load i8*** %2
	%4 = getelementptr inbounds i8** %3, i32 1
	%5 = load i8** %4
	%6 = call i32 @atoi(i8* %5)
	store i32 %6, i32* %x
	%7 = load i8*** %2
	%8 = getelementptr inbounds i8** %7, i32 2
	%9 = load i8** %8
	%10 = call i32 @atoi(i8* %9)
	store i32 %10, i32* %y
	store i32 0, i32* %i
	br label %lbl0
	lbl0:
	%11 = load i32* %i
	%12 = load i32* %x
%13 = icmp sle i32 %11, %12
%14 = zext i1 %13 to i32
	%15 = trunc i32 %14 to i1	br i1 %15, label %lbl1 , label %lbl2
	lbl1:
	store i32 0, i32* %j
	br label %lbl3
	lbl3:
	%16 = load i32* %j
	%17 = load i32* %y
%18 = icmp sle i32 %16, %17
%19 = zext i1 %18 to i32
	%20 = trunc i32 %19 to i1	br i1 %20, label %lbl4 , label %lbl5
	lbl4:
	%21 = load i32* %i
	%22 = getelementptr inbounds [20 x i8]* %buf, i32 0, i32 0
	%23 = call i8* @itoa(i32 %21, i8* %22)
	%24 = call i32 @puts(i8* %23)
	%25 = load i32* %j
	%26 = add i32 %25, 1
	store i32 %26, i32* %j
	br label %lbl3
	lbl5:
	%27 = load i32* %i
	%28 = add i32 %27, 1
	store i32 %28, i32* %i
	br label %lbl0
	lbl2:
	ret i32 0
}
