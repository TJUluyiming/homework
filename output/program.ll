; ModuleID = 'sysy2026_compiler'
source_filename = "test/test_wrs.sy"

@a = global i32 10
declare i32 @getint()

declare i32 @getch()

declare i32 @getarray(i32*)

declare void @putint(i32)

declare void @putch(i32)

declare void @putarray(i32, i32*)

declare void @starttime()

declare void @stoptime()

define i32 @main() {
label_ENTRY:
  store i32 10, i32* @a
  ret i32 0
}
