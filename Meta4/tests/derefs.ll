; ModuleID = 'derefs.c'
target datalayout = "e-m:e-p:32:32-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

; Function Attrs: nounwind
define i32 @main2(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i8**, align 4
  %buffer = alloca [20 x i8], align 1
  %buf = alloca i8*, align 4
  %int_buffer = alloca i32**, align 4
  %a = alloca i8, align 1
  store i32 %argc, i32* %1, align 4
  store i8** %argv, i8*** %2, align 4
  %3 = getelementptr inbounds [20 x i8]* %buffer, i32 0, i32 0
  %4 = load i8* %a, align 1
  %5 = sext i8 %4 to i32
  %6 = getelementptr inbounds i8* %3, i32 %5
  %7 = load i8* %6, align 1
  %8 = load i8* %a, align 1
  %9 = sext i8 %8 to i32
  %10 = load i8** %buf, align 4
  %11 = getelementptr inbounds i8* %10, i32 %9
  %12 = load i8* %11, align 1
  ret i32 0
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.5.0-4ubuntu2~trusty2 (tags/RELEASE_350/final) (based on LLVM 3.5.0)"}
