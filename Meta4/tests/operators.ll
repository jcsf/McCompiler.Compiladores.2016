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
define i32 @main2 () {
	%a = alloca i32
	%b = alloca i32
	%c = alloca i8
	%d = alloca i8
	%e = alloca i32*
	%f = alloca i32**
	%g = alloca i32*
	%h = alloca i8*
	%j = alloca i32**
	%buf = alloca [10 x i8]
	%buffer = alloca [20 x i32]
	%k = alloca i8**
	%l = alloca i8**
	%m = alloca i8*
	%n = alloca i32***
	%o = alloca i32***
	%1 = getelementptr inbounds [20 x i32]* %buffer, i32 0, i32 0
	%2 = load i32* %a
	%3 = getelementptr inbounds i32 %1, i32 %2
	ret i32 0
	ret i32 0
}
